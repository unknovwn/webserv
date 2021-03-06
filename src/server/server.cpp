
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

bool print_dir(std::string& path, std::string& root);
std::string generate_error_page(int error_code);

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
  :listen_("127.0.0.1:80"), server_names_() {
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
//=============================== SERV NAME ====================================
const std::vector<string> &Server::GetServerNames() const {
  return server_names_;
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

void Server::AddLocation(const Location &new_loc) {
  for (const auto &x : routes_) {
    if (x.GetUri() == new_loc.GetUri()) {
      throw Server::Exception();
    }
  }
  routes_.push_back(new_loc);
}

//============================= RESPONSE ======================================

Response* Server::CreateResponse(Request &request) const {
  auto        location = FindLocation(request.GetPath());
  std::string absolute_path(GetAbsolutePath(request.GetPath()));

  if (location) {
    if (!location->GetMethods().empty() && !MethodIsAllowed(request,
                                                            *location)) {
      std::cerr << absolute_path << ": method not allowed" << std::endl;
      auto response = new Response(Response::kMethodNotAllowed);
      response->AddToBody(GetErrorPageBody(Response::kMethodNotAllowed));
      return response;
    }
    if (!location->GetRoot().empty()) {
      absolute_path = GetAbsolutePathFromLocation(request, *location);
    }
  }
  Response* response = nullptr;
  auto method = request.GetMethod();
  if (method == "GET") {
    response = ResponseFromGet(request, absolute_path, location);
  } else if (method == "POST") {
    response = ResponseFromPost(request, absolute_path, location);
  } else if (method == "PUT") {
    response = ResponseFromPut(request, absolute_path, location);
  } else if (method == "HEAD") {
    response = ResponseFromHead(request, absolute_path, location);
  }
//  response->ResizeBody(GetMaxBodySize());
  return response;
}

std::string Server::GetErrorPageBody(int status_code) const {
  std::map<int, std::string>::const_iterator it
    = error_pages_.find(status_code);
  if (it != error_pages_.end()) {
    try {
      return FileToString((*it).second);
    } catch (FileDoesNotExist& e) {}
  }
  return generate_error_page(status_code);
}

Response* Server::CreateBadRequestResponse() const {
  auto response = new Response(Response::kBadRequest);
  response->AddToBody(GetErrorPageBody(Response::kBadRequest));
  return response;
}

// Response from methods
Response* Server::ResponseFromGet([[maybe_unused]] Request &request,
                                  const std::string &path,
                                  const Location *location = nullptr) const {
  if (location && (IsDir(path.c_str())
        || (path.find(location->GetUri()) != std::string::npos))) {
    try {
      return ResponseFromLocationIndex(*location, path);
    } catch (FileDoesNotExist &e) {
      if (location->GetAutoindex()) {
        return ResponseFromAutoIndex(path, path);
      }
      std::cerr << e.what() << std::endl;
      auto response = new Response(Response::kNotFound);
      response->AddToBody(GetErrorPageBody(Response::kNotFound));
      return response;
    }
  }
  std::string body;

  try {
    body.append((FileToString(path.c_str())));
  } catch (FileDoesNotExist & e) {
//    std::cout << e.what() << std::endl;
    auto response = new Response(Response::kNotFound);
    response->AddToBody(GetErrorPageBody(Response::kNotFound));
    return response;
  }
  auto response = new Response(Response::kOk);

  response->AddToBody(body);
  response->AddHeader("Content-Type", GetContentType(path));
  return response;
}

Response* Server::ResponseFromHead(Request &request, const std::string &path,
    const Location *location = nullptr) const {
  auto response = ResponseFromGet(request, path, location);

  response->ClearBody();
  return response;
}

Response* Server::ResponseFromPut(Request &request, const std::string &path,
    [[maybe_unused]] const Location *location = nullptr) const {
  struct stat   file_stat;
  Response      *response;

//  MakeDir(CropFromLastSymbol(path, '/').c_str());
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
    auto response = new Response(Response::kForbidden);
    response->AddToBody(GetErrorPageBody(Response::kForbidden));
    return response;
  }
  // Response
//  file.write(request.GetBody().c_str(), request.GetBody().length());
  file << request.GetBody();
  response->AddHeader("Content-Length", "0");
  response->AddHeader("Content-Type", GetContentType(path));
  file.close();
  return response;
}

Response *Server::ResponseFromPost(Request &request, const std::string &path,
                                   const Location *location) const {
  size_t dot = path.rfind('.');

  if (!location || dot == std::string::npos ||
      path.substr(dot) != location->GetCgiExtension()) {
    if (request.GetBody().size() > location->GetMaxBodySize()) {
      return new Response(Response::kPayloadTooLarge);
    }
    return new Response(Response::kOk);
  }

  Cgi cgi(location->GetCgiPath());

  return new Response(cgi.CreateResponse(request, location->GetMaxBodySize()));
}

// Response Utils
Response *Server::ResponseFromLocationIndex(const Location &location,
                                            const std::string &path) {
  struct stat file_stat;
//  std::string root;
  auto        indexes(location.GetIndex());

//  if (!location.GetRoot().empty()) {
//    root = JoinPath(GetAbsolutePath(), location.GetRoot());
//  } else {
//    root = JoinPath(GetAbsolutePath(), location.GetUri());
//  }

  auto index = std::find_if(indexes.begin(), indexes.end(),
               [&](const std::string &index) {
    std::string index_path(JoinPath(path, index));
    if (stat(index_path.c_str(), &file_stat)) {
      return false;
    }
    return true;});

  if (index == indexes.end()) {
    if (location.GetAutoindex()) {
      return ResponseFromAutoIndex(path, path);
    }
    throw FileDoesNotExist("index files");
  }
  auto response = new Response(Response::kOk);

  response->AddToBody(FileToString(JoinPath(path, index->c_str()).c_str()));
  response->AddHeader("Content-Type", GetContentType(*index));
  return response;
}

Response *Server::ResponseFromAutoIndex(std::string absolute_path,
                                        std::string relative_path) {
  if (!print_dir(absolute_path, relative_path)) {
    return new Response(Response::kNotFound);
  }
  auto response = new Response(Response::kOk);

  response->AddToBody(FileToString("autoindex.html"));
  response->AddHeader("Content-Length", std::to_string(
          response->get_body().length()));
  response->AddHeader("Content-Type", "text/html");
  return response;
}

// Element Access
const Location* Server::FindLocation(const std::string &path) const {
  auto location =  std::find_if(routes_.rbegin(),
                                routes_.rend(),
                                [&path](const Location& l) {
    bool equal = l.GetUri().compare(0, l.GetUri().length() - 1,
                              path, 0, l.GetUri().length() - 1) == 0;
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
  std::string location_uri(location.GetUri());

  if (location_uri.length() > 1 && location_uri.back() == '/') {
    location_uri.pop_back();
  }
  std::string path(request.GetPath());
  size_t found = path.find(location_uri);

  path.erase(found, location_uri.length());
  if (!location.GetRoot().empty()) {
    path = JoinPath(location.GetRoot(), path);
    return GetAbsolutePath(path);
  }
  return GetAbsolutePath(path);
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
    throw FileDoesNotExist(filename);
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

  return type == content_types.end() ? "text/plain" : content_types[ext];
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

bool Server::IsDir(const std::string &path) {
  struct stat file_stat;

  stat(path.c_str(), &file_stat);
  return S_ISDIR(file_stat.st_mode) != 0;
}

bool Server::FileExist(const std::string &path) {
  struct stat file_stat;

  return stat(path.c_str(), &file_stat) == 0;
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
