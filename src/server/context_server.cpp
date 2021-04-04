#include "context_server.hpp"

//=============================== HOST =========================================
const std::string &server::GetHost() const {
  return host_;
}
void server::SetHost(const std::string &host) {
  server::host_ = host;
}
//=============================== PORT =========================================
int server::GetPort() const {
  return port_;
}
void server::SetPort(int port) {
  port_ = port;
}
//=============================== ERROR PAGES ==================================
std::map<int, string> &server::GetErrorPages() {
  return error_pages_;
}
//=============================== SERV NAME ====================================
const std::string &server::GetServName() const {
  return serv_name_;
}
void server::SetServName(const std::string &serv_name) {
  serv_name_ = serv_name;
}
//============================== LOCATION ======================================
const location* server::GetLocation(const string &uri) const {
  const location *best_match = nullptr;
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

void server::AddLocation(const location &new_loc) {
  for (const auto &x : routes_)
    if (x.GetUri() == new_loc.GetUri())
      throw ;
    routes_.push_back(new_loc);
}
