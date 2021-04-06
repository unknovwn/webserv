#pragma once

#include <string>
#include <vector>
#include <map>
#include "../location/location.hpp"

using string = std::string;

class Server {
  string                        listen_;
  string                        server_name_;
  std::vector<Location>         routes_;
  std::map<int, string>         error_pages_;

 public:
  Server();
  Server& operator=(const Server &other)  = default;
  Server(const Server &other)             = default;
  ~Server()                               = default;

  const string&                 GetListen() const;
  void                          SetListen(const string &listen);
  const string&                 GetServName() const;
  void                          SetServName(const string &serv_name);
  const std::vector<Location>&  GetAll_Loc();
  const Location*               GetLocation(const string &uri) const;
  void                          AddLocation(const Location &new_loc);
  string                        GetErrorPage(int error_val);
  void                          SetErrorPage(int error_val, string &error_file);
// std::map<int, string>&      qGetErrorPages();
// если заданы то подтягиваем файлы, елси нет - генерим свои
  class Exception: public std::exception {
   public:
    virtual const char* what() const throw();
  };
};