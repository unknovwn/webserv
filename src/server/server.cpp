
#include <unistd.h>
#include <sys/stat.h>

#include <memory>
#include <algorithm>
#include <fstream>
#include <functional>

#include "server.hpp"
#include "response.hpp"
#include "request.hpp"
#include "cgi.hpp"

#define MB 1048576

int     print_dir(std::string path, std::string root);

std::map
        <std::string,
        std::function<Response*(Request&, const std::string&,
                                const Location*)> >
                                               Server::response_from_methods = {
        {"GET", Server::ResponseFromGet},
        {"HEAD", Server::ResponseFromHead},
        {"PUT", Server::ResponseFromPut},
        {"POST", Server::ResponseFromPost}
};

std::map<std::string, std::string> Server::content_types = {
        {".html", "text/html"},
        {".css", "text/css"},
        {".js", "text/javascript"},
        {".gif", "image/gif"},
        {".jpg", "image/jpeg"},
        {".jpeg", "image/jpeg"},
        {".png", "image/png"}
};

Server::Server()
  :listen_("127.0.0.1:80"), server_names_(), max_body_size_(1 * MB) {
  server_names_.emplace_back("intra42.fr");
}
//=============================== LISTEN =======================================
const string &Server::GetListen() const {
  return this->listen_;
}
void Server::SetListen(const string &listen) {
  this->listen_ = listen;
}
//=============================== ERROR PAGES ==================================
string Server::GetErrorPage(int error_val) const {
  auto it = error_pages_.find(error_val);
  if (it == error_pages_.end())
    throw Server::Exception();
  return (*it).second;
}
void Server::SetErrorPage(int error_val, string &error_file) {
  auto it = error_pages_.find(error_val);
  if ((*it).first == error_val)
    throw Server::Exception();
  error_pages_.insert(std::pair<int, string>(error_val, error_file));
}
//============================== MAX BODY SIZE =================================
int Server::GetMaxBodySize() const {
  return this->max_body_size_;
}
void Server::SetMaxBodySize(int max_body_size) {
  this->max_body_size_ = max_body_size;
}
//=============================== SERV NAME ====================================
const std::vector<string> &Server::GetServerNames() const {
  return server_names_;
}

bool Server::FindServerName(string &name) {
  for (const auto &x : server_names_) {
    if (x == name)
      return true;
  }
  return false;
}

void  Server::AddServerName(const string& name) {
  for (const auto &x : server_names_) {
    if (x == name)
      return;
  }
  server_names_.push_back(name);
}
//============================== Location ======================================
const std::vector<Location> &Server::GetLocations() const {
  return this->routes_;
}

const Location* Server::GetLocation(const string &uri) const {
  const Location *best_match = nullptr;
  size_t  depth;
  size_t  max_depth = 0;

  for (const auto &x : routes_) {
    if (uri.starts_with(x.GetUri())) {
      depth = std::count(x.GetUri().begin(), x.GetUri().end(), '/');
      if (depth > max_depth) {
        max_depth = depth;
        best_match = &x;
      }
    }
  }
  return best_match;
}

void Server::AddLocation(const Location &new_loc) {
  for (const auto &x : routes_) {
    if (x.GetUri() == new_loc.GetUri()) {
      throw Server::Exception();
    }
  }
  routes_.push_back(new_loc);
}
//============================= PRINT =========================================
void Server::Print() const {
  std::cout << "listen: " << listen_ << std::endl;
  std::cout << "server_names: ";
  for (const auto &x : server_names_)
    std::cout << x;
  std::cout << std::endl;
  std::cout << "locations:" << std::endl;
  for (auto& route : routes_) {
    route.Print();
  }
  std::cout << "error_pages:" << std::endl;
  for (auto& error_page : error_pages_) {
    std::cout << error_page.first << ": " << error_page.second << std::endl;
  }
  std::cout << "max_body_size: " << max_body_size_ << std::endl;
}

//============================= RESPONSE ======================================

Response* Server::CreateResponse(Request &request) const {
  // Find Location
  auto location = FindLocation(request.GetPath());

  // Getting info
  size_t found_root = request.GetPath().find(GetRealRoot(), 0);
  std::string full_path;

  if (found_root == std::string::npos) {
    full_path = GetRealRoot() + request.GetPath();
  } else {
    full_path = request.GetPath();
  }
  // Call Method Handler
  return response_from_methods[request.GetMethod()](request, full_path,
                                                    location);
}

Response* Server::CreateBadRequestResponse() {
  return new Response(Response::kBadRequest);
}

Response* Server::ResponseFromGet(Request &request,
                                  const std::string &path,
                                  const Location *location = nullptr) {
  (void)(request);
  Response    *response(nullptr);
  struct stat file_stat;

  // If math location
  if (location) {
    response = GetResponseFromLocationIndex(*location);
    if (response) {
      return response;
    }
    if (location->GetAutoindex()) {
      if (print_dir(path, request.GetPath())) {
        return new Response(Response::kNotFound);
      }
      response = new Response(Response::kOk);
      response->AddToBody(FileToString("autoindex.html"));
      response->AddHeader("Content-Length",std::to_string(
                                                response->get_body().length()));
      response->AddHeader("Content-Type", "text/html");
      return response;
    }
  }

  std::string body;
  try {
    body.append((FileToString(path.c_str())));
  } catch (...) {
    return new Response(Response::kNotFound);
  }
  response = new Response(Response::kOk);
  response->AddToBody(body);
  response->AddHeader("Content-Length",std::to_string(
          response->get_body().length()));
  response->AddHeader("Content-Type", GetContentType(path));
  return response;
  return response;
}

Response* Server::ResponseFromHead(Request &request,
                                   const std::string &path,
                                   const Location *location = nullptr) {
  (void)(request);
  Response    *response(nullptr);
  struct stat file_stat;

  if (location) {
    response = GetResponseFromLocationIndex(*location);
    if (response) {
      response->ClearBody();
      return response;
    }
  }

  // 404
  if ((location && !location->GetIndex().empty()) ||
      stat(path.c_str(), &file_stat)) {
    std::cout << "404 NOT FOUND" << std::endl;
    return new Response(Response::kNotFound);
  }

  // Response
  response = new Response(Response::kOk);
  response->AddHeader("Content-Length", std::to_string(file_stat.st_size));
  response->AddHeader("Content-Type", GetContentType(path));
  return response;
}

Response* Server::ResponseFromPut(Request &request,
                                  const std::string &path,
                                  const Location *location = nullptr) {
  (void)(location);
  struct stat   file_stat;
  Response      *response;

  // If file not exist
  if (stat(path.c_str(), &file_stat)) {
    response = new Response(Response::kCreated);
    response->AddHeader("Content-Location", path);
  } else {
    response = new Response(Response::kNoContent);
  }
  std::ofstream file(path.c_str());

  // Error
  if (!file.is_open()) {
    std::cerr << "ERROR: can't open/create file" << std::endl;
    delete response;
    return new Response(Response::kForbidden);
  }
  // Response
  stat(path.c_str(), &file_stat);
  file.write(request.GetBody().c_str(), request.GetBody().length());
  response->AddHeader("Content-Length", std::to_string(file_stat.st_size));
  response->AddHeader("Content-Type", GetContentType(path));
  return response;
}

Response *Server::ResponseFromPost(Request &request, const std::string &path,
                                   const Location *location) {
  size_t dot = path.rfind('.');

  if (!location || dot == std::string::npos) {
    return new Response(Response::kOk);
  }
  if (path.substr(dot) != location->GetCgiExtension()) {
    return new Response(Response::kOk);
  }
  Cgi cgi(location->GetCgiPath());

  return new Response(cgi.CreateResponse(request));
}

Response *Server::GetResponseFromLocationIndex(const Location &location) {
  struct stat file_stat;

  for(auto &i : location.GetIndex()) {
    std::string real_path(JoinPath(GetRealRoot(), location.GetUri()));
    real_path = JoinPath(real_path, i);
    if (!stat(real_path.c_str(), &file_stat)) {
      auto response = new Response(Response::kOk);
      response->AddToBody(FileToString(real_path.c_str()));
      response->AddHeader("Content-Length",std::to_string(
                                                response->get_body().length()));
      response->AddHeader("Content-Type", GetContentType(real_path));
      return response;
    }
  }
  return nullptr;
}

const Location* Server::FindLocation(std::string path) const {
  auto location =  std::find_if(routes_.rbegin(),
                                routes_.rend(),
                                [&path](const Location& l) {
    bool equal = l.GetUri().compare(0, l.GetUri().length(),
                              path, 0, l.GetUri().length()) == 0;
    if (equal && path.length() > l.GetUri().length()) {
      return path.at(l.GetUri().length()) == '/';
    }
    return equal;
  });
  return location == routes_.rend() ? nullptr : &(*location);
}

std::string Server::JoinPath(const std::string &a, const std::string &b) {
  if (a.empty() || b.empty()) {
    return a.empty() ? b : a;
  }
  if (a.back() == '/' && b.front() == '/') {
    return a.substr(0, a.length() - 1) + b;
  }
  if (a.back() != '/' && b.front() != '/') {
    return a + "/" + b;
  }
  return a + b;
}

std::string Server::GetRealRoot() {
  char pwd[1024];

  getcwd(pwd, 1024);
  return pwd;
}

std::string Server::FileToString(const char *filename) {
  std::ifstream t(filename);
  if (!t.is_open()) {
    throw (std::string ("not found"));
  }
  std::string   str((std::istreambuf_iterator<char>(t)),
                    std::istreambuf_iterator<char>());

  return str;
}

std::string Server::GetContentType(const std::string &filename) {
  size_t found_index = filename.rfind('.');

  if (found_index == std::string::npos) {
    return std::string("text/plain");
  }
  std::string ext(filename.substr(found_index));
  std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
  return content_types[ext];
}

//========================== EXCEPTION =========================================
const char *Server::Exception::what() const noexcept {
  return ("Server context Exception\n");
}
