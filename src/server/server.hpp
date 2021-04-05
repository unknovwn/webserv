#pragma once

#include <string>
#include <vector>
#include <map>
#include "../location/location.hpp"

using string = std::string;

class Server {
  string                        listen_;
  string                        server_name_;
  string                        host_; // == serv_name?
  std::vector<Location>         routes_;
  std::map<int, string>         error_pages_;
 public:
  Server& operator=(Server &other);

  const string &                GetListen() const;
  void                          SetListen(const string &listen);
  const string&                 GetServName() const;
  void                          SetServName(const std::string &serv_name);
  const string&                 GetHost() const;
  void                          SetHost(const std::string &host);
  const Location*               GetLocation(const string &uri) const;
  void                          AddLocation(const Location &new_loc);
  std::map<int, string>&        GetErrorPages();

  // если заданы то подтягиваем файлы, елси нет - генерим свои
};
