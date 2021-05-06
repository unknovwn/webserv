
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

//bool Server::FindServerName(string &name) {
//  for (const auto &x : server_names_) {
//    if (x == name)
//      return true;
//  }
//  return false;
//}

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

//const Location* Server::GetLocation(const string &uri) const {
//  const Location *best_match = nullptr;
//  size_t  depth;
//  size_t  max_depth = 0;
//
//  for (const auto &x : routes_) {
//    if (uri.starts_with(x.GetUri())) {
//      depth = std::count(x.GetUri().begin(), x.GetUri().end(), '/');
//      if (depth > max_depth) {
//        max_depth = depth;
//        best_match = &x;
//      }
//    }
//  }
//  return best_match;
//}

void Server::AddLocation(const Location &new_loc) {
  for (const auto &x : routes_) {
    if (x.GetUri() == new_loc.GetUri()) {
      throw Server::Exception();
    }
  }
  routes_.push_back(new_loc);
}
//============================= PRINT =========================================
//void Server::Print() const {
//  std::cout << "listen: " << listen_ << std::endl;
//  std::cout << "server_names: ";
//  for (const auto &x : server_names_)
//    std::cout << x;
//  std::cout << std::endl;
//  std::cout << "locations:" << std::endl;
//  for (auto& route : routes_) {
//    route.Print();
//  }
//  std::cout << "error_pages:" << std::endl;
//  for (auto& error_page : error_pages_) {
//    std::cout << error_page.first << ": " << error_page.second << std::endl;
//  }
//  std::cout << "max_body_size: " << max_body_size_ << std::endl;
//}

//============================= RESPONSE ======================================

Response* Server::CreateResponse(Request &request) const {
  auto        location = FindLocation(request.GetPath());
  std::string absolute_path(GetAbsolutePath(request.GetPath()));

  if (location) {
    if (!location->GetMethods().empty() && !MethodIsAllowed(request,
                                                            *location)) {
      // TODO(gdrive): error page
      std::cerr << absolute_path << ": method not allowed" << std::endl;
      return new Response(Response::kMethodNotAllowed);
    }
    if (!location->GetRoot().empty()) {
      absolute_path = GetAbsolutePathFromLocation(request, *location);
    }
  }
  auto response = response_from_methods[request.GetMethod()](request,
                                                             absolute_path,
                                                             location);
  response->ResizeBody(GetMaxBodySize());
  return response;
}

Response* Server::CreateBadRequestResponse() {
  return new Response(Response::kBadRequest);
}

// Response from methods
Response* Server::ResponseFromGet([[maybe_unused]] Request &request,
                                  const std::string &path,
                                  const Location *location = nullptr) {
  if (location && !location->GetIndex().empty()) {
    try {
      return ResponseFromLocationIndex(*location);
    } catch (FileDoesNotExist &e) {
      std::cerr << e.what() << std::endl;
    }
  }
  std::string body;

  try {
    body.append((FileToString(path.c_str())));
  } catch (FileDoesNotExist & e) {
    // TODO(gdrive): error page
    if (location && location->GetAutoindex()) {
      return ResponseFromAutoIndex(path, path);
    }
    return new Response(Response::kNotFound);
  }
  auto response = new Response(Response::kOk);

  response->AddToBody(body);
  response->AddHeader("Content-Type", GetContentType(path));
  return response;
}

Response* Server::ResponseFromHead(Request &request,
                                   const std::string &path,
                                   const Location *location = nullptr) {
  auto response = ResponseFromGet(request, path, location);

  response->ClearBody();
  return response;
}

Response* Server::ResponseFromPut(Request &request,
                          const std::string &path,
                          [[maybe_unused]] const Location *location = nullptr) {
  struct stat   file_stat;
  Response      *response;

  MakeDir(CropFromLastSymbol(path, '/').c_str());
  if (stat(path.c_str(), &file_stat)) {
    response = new Response(Response::kCreated);
    response->AddHeader("Content-Location", path);
  } else {
    response = new Response(Response::kNoContent);
  }
  std::ofstream file(path.c_str());

  if (!file.is_open()) {
    std::cerr << path.c_str() <<  ": can't open/create " << std::endl;
    delete response;
    // TODO(gdrive): error page
    return new Response(Response::kForbidden);
  }
  // Response
  stat(path.c_str(), &file_stat);
  file.write(request.GetBody().c_str(), request.GetBody().length());
  response->AddHeader("Content-Length", std::to_string(file_stat.st_size));
  response->AddHeader("Content-Type", GetContentType(path));
  file.close();
  return response;
}

Response *Server::ResponseFromPost(Request &request, const std::string &path,
                                   const Location *location) {
  size_t dot = path.rfind('.');

  if (!location || dot == std::string::npos ||
      path.substr(dot) != location->GetCgiExtension()) {
    return new Response(Response::kOk);
  }
  Cgi cgi(location->GetCgiPath());

  return new Response(cgi.CreateResponse(request));
}

// Response Utils
Response *Server::ResponseFromLocationIndex(const Location &location) {
  struct stat file_stat;
  std::string root;
  auto        indexes(location.GetIndex());

  if (!location.GetRoot().empty()) {
    root = JoinPath(GetAbsolutePath(), location.GetRoot());
  } else {
    root = JoinPath(GetAbsolutePath(), location.GetUri());
  }
  auto index = std::find_if(indexes.begin(), indexes.end(),
               [&](const std::string &index) {
    std::string path(JoinPath(root, index));
    if (stat(path.c_str(), &file_stat)) {
      return false;
    }
    return true;});

  if (index == indexes.end()) {
    if (location.GetAutoindex()) {
      return ResponseFromAutoIndex(root, root);
    }
    throw FileDoesNotExist("index files");
  }
  auto response = new Response(Response::kOk);

  response->AddToBody(FileToString(JoinPath(root, index->c_str()).c_str()));
  response->AddHeader("Content-Type", GetContentType(*index));
  return response;
}

Response *Server::ResponseFromAutoIndex(std::string absolute_path,
                                        std::string relative_path) {
  if (print_dir(absolute_path, relative_path)) {
    return new Response(Response::kNotFound);
  }
  auto response = new Response(Response::kOk);

  response->AddToBody(FileToString("autoindex.html"));
  response->AddHeader("Content-Length",std::to_string(
          response->get_body().length()));
  response->AddHeader("Content-Type", "text/html");
  return response;
}

// Element Access
const Location* Server::FindLocation(std::string path) const {
  auto location =  std::find_if(routes_.rbegin(),
                                routes_.rend(),
                                [&path](const Location& l) {
    bool equal = l.GetUri().compare(0, l.GetUri().length(),
                              path, 0, l.GetUri().length()) == 0;
    return equal;
  });
  return location == routes_.rend() ? nullptr : &(*location);
}

// Info
bool Server::MethodIsAllowed(Request &request, const Location &location) {
  return !(std::find(location.GetMethods().begin(), location.GetMethods().end(),
                   request.GetMethod()) == location.GetMethods().end());
}

// Utils
std::string Server::GetAbsolutePath() {
  char pwd[1024];

  getcwd(pwd, 1024);
  return pwd;
}

std::string Server::GetAbsolutePath(const std::string &relative_path) {
  size_t found_root = relative_path.find(GetAbsolutePath(), 0);

  if (found_root == std::string::npos) {
    return JoinPath(GetAbsolutePath(), relative_path);
  }
  return relative_path;
}

std::string Server::GetAbsolutePathFromLocation(Request &request,
                                                const Location &location) {
  if (location.GetRoot().empty()) {
    return GetAbsolutePath(request.GetPath());
  }
  std::string absolute_path(
                          request.GetPath().substr(location.GetUri().length()));
  absolute_path = JoinPath(location.GetRoot(), absolute_path);
  return GetAbsolutePath(absolute_path);
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

std::string Server::FileToString(const std::string &filename) {
  struct stat file_stat;

  if (filename.back() == '/' || stat(filename.c_str(), &file_stat)) {
    throw FileDoesNotExist(filename);
  }
  if (S_ISDIR(file_stat.st_mode)) {
    throw FileDoesNotExist(filename);
  }
  std::ifstream file(filename);

  if (!file.is_open()) {
    throw (FileDoesNotExist(filename));
  }
  std::string   str((std::istreambuf_iterator<char>(file)),
                    std::istreambuf_iterator<char>());
  file.close();
  return str;
}

std::string Server::GetContentType(const std::string &filename) {
  size_t found_index = filename.rfind('.');

  if (found_index == std::string::npos) {
    return std::string("text/plain");
  }
  std::string ext(filename.substr(found_index));

  std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
  auto type = content_types.find(ext);

  return type == content_types.end() ? "" : content_types[ext];
}

std::string Server::CropFromLastSymbol(const std::string &str, char c) {
  size_t found = str.find_last_of(c);

  if (found == std::string::npos) {
    return std::string(str);
  }
  return str.substr(0, found);
}

int Server::MakeDir(const char *path) {
  return mkdir(path, 0755);
}

// Exceptions
const char *Server::Exception::what() const noexcept {
  return ("Server context Exception\n");
}

Server::FileDoesNotExist::FileDoesNotExist(const std::string &file_name)
: file_name_(file_name) {}

const char *Server::FileDoesNotExist::what() const noexcept {
  std::string what(file_name_);

  return what.append(" does not exist").c_str();
}
