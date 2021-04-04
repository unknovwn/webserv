#pragma once

#include <string>
#include <vector>
#include <map>
#include "../location/context_location.hpp"

using string = std::string;

class server {
  string                        host_;
  int                           port_;
  string                        serv_name_;
  std::vector<location>         routes_;
  std::map<int, string>         error_pages_;
 public:
  const string&                 GetHost() const;
  void                          SetHost(const std::string &host);
  int                           GetPort() const;
  void                          SetPort(int port);
  const string&                 GetServName() const;
  void                          SetServName(const std::string &serv_name);
  const location*               GetLocation(const string &uri) const;
  void                          AddLocation(const location &new_loc);
  std::map<int, string>&        GetErrorPages(); // если заданы то подтягиваем файлы, елси нет - генерим свои
};
