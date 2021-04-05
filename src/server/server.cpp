#include "server.hpp"
#include <utility>

Server::Server()
  :listen_("127.0.0.1:80"), server_name_("intra42.fr"),
  routes_(), error_pages_()
{}
//=============================== LISTEN =======================================
const string &Server::GetListen() const {
  return this->listen_;
}
void Server::SetListen(const string &listen) {
  this->listen_ = listen;
}
//=============================== ERROR PAGES ==================================
//  std::map<int, string> &Server::GetErrorPages() {
//  return this->error_pages_;
//}
string Server::GetErrorPage(int error_val) {
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
const string &Server::GetServName() const {
  return this->server_name_;
}
void Server::SetServName(const string &serv_name) {
  this->server_name_ = serv_name;
}
//============================== Location ======================================
const std::vector<Location> &Server::GetAll_Loc() {
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
  for (const auto &x : routes_)
    if (x == new_loc)
      throw Server::Exception();
    routes_.push_back(new_loc);
}
//========================== EXCEPTION =========================================
const char *Server::Exception::what() const throw() {
  return ("Server context Exception\n");
}
