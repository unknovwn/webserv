#include "context_server.hpp"

const std::string &server::GetHost() const {
  return host_;
}
void server::SetHost(const std::string &host) {
  server::host_ = host;
}

int server::GetPort() const {
  return port_;
}
void server::SetPort(int port) {
  port_ = port;
}

const std::string &server::GetErrorPages() const {
  return error_pages_;
}
void server::SetErrorPages(const std::string &error_pages) {
  error_pages_ = error_pages;
}

const std::string &server::GetIpAdress() const {
  return ip_adress_;
}
void server::SetIpAdress(const std::string &ip_adress) {
  ip_adress_ = ip_adress;
}

const std::string &server::GetServName() const {
  return serv_name_;
}
void server::SetServName(const std::string &serv_name) {
  serv_name_ = serv_name;
}

const location& server::GetRoutes(size_t id) const {
  return routes_[id];
}
void server::SetRoutes(const std::vector<location> &routes) {
  routes_ = routes;
}
