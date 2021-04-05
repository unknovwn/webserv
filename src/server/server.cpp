#include "server.hpp"

Server &Server::operator=(Server &other) {
  listen_       = other.listen_;
  server_name_  = other.server_name_;
  routes_       = other.routes_;
  error_pages_  = other.error_pages_;
  return (*this);
}
//=============================== HOST =========================================
const std::string &Server::GetHost() const {
  return this->host_;
}
void Server::SetHost(const std::string &host) {
  Server::host_ = host;
}
//=============================== LISTEN =======================================
const string &Server::GetListen() const {
  return this->listen_;
}
void Server::SetListen(const std::string &listen) {
  this->listen_ = listen;
}
//=============================== ERROR PAGES ==================================
std::map<int, string> &Server::GetErrorPages() {
  return this->error_pages_;
}
//=============================== SERV NAME ====================================
const std::string &Server::GetServName() const {
  return this->server_name_;
}
void Server::SetServName(const std::string &serv_name) {
  this->server_name_ = serv_name;
}
//============================== Location ======================================
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
  for (const auto &x : routes_)
    if (x.GetUri() == new_loc.GetUri())
      throw ;
    routes_.push_back(new_loc);
}
