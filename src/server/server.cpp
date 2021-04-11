#include "server.hpp"
#define MB 1000000

Server::Server()
  :listen_("127.0.0.1:80"), max_body_size_(1 * MB) {
  server_names_[0] = "intra42.fr";
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

//========================== EXCEPTION =========================================
const char *Server::Exception::what() const throw() {
  return ("Server context Exception\n");
}
