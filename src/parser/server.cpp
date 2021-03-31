#include "server.hpp"

Server::Server() {
  this->port_ = -1;
  this->host_ = "";
  this->root_ = "/data/www";
  this->client_max_body_size_ = (1 << 20);
}

Server::Server(const Server& other) {
  this->port_ = other.port_;
  this->root_ = other.root_;
  this->client_max_body_size_ = other.client_max_body_size_;
  this->locations_ = other.locations_;
  this->server_names_ = other.server_names_;
  this->host_ = other.host_;
}

Server::~Server() {}

Server& Server::operator=(const Server& other) {
  if (this == &other) {
    return *this;
  }
  this->port_ = other.port_;
  this->root_ = other.root_;
  this->client_max_body_size_ = other.client_max_body_size_;
  this->locations_ = other.locations_;
  this->server_names_ = other.server_names_;
  this->host_ = other.host_;
  return *this;
}

const Server::locations_t& Server::locations() const {
  return this->locations_;
}

const Server::server_names_t& Server::server_names() const {
  return this->server_names_;
}

const std::string& Server::root() const {
  return this->root_;
}

const Server::host_t& Server::host() const {
  return this->host_;
}

size_t Server::client_max_body_size() const {
  return this->client_max_body_size_;
}

int Server::port() const {
  return this->port_;
}

const char* Server::InvalidHostException::what() const throw() {
  return "Invalid server host";
}

const char* Server::InvalidPortException::what() const throw() {
  return "Invalid server port";
}

const char* Server::InvalidServerNamesException::what() const throw() {
  return "Invalid server names";
}

const char* Server::InvalidClientMaxBodySizeException::what() const throw() {
  return "Invalid client max body size";
}

const char* Server::NoListenPortException::what() const throw() {
  return "No listen port specified";
}
