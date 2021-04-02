#pragma once

#include <string>
#include <vector>
#include "../location/context_location.hpp"

class server {
  std::string             host_;
  int                     port_;
  std::string             error_pages_;
  std::string             serv_name_;
  std::vector<location>   routes_;
 public:
  const std::string&      GetHost() const;
  void                    SetHost(const std::string &host);
  int                     GetPort() const;
  void                    SetPort(int port);
  const std::string&      GetErrorPages() const;
  void                    SetErrorPages(const std::string &error_pages);
  const std::string&      GetServName() const;
  void                    SetServName(const std::string &serv_name);
  const location&         GetRoutes(size_t id) const;
  void                    SetRoutes(const std::vector<location> &routes);
};
