#pragma once

#include <memory>
#include <string>
#include <vector>
#include <map>
#include <utility>
#include <functional>

#include "location.hpp"

using string = std::string;

class Request;
class Response;

class Server {
  string                        listen_;
  std::vector<string>           server_names_;
  std::vector<Location>         routes_;
  std::map<int, string>         error_pages_;
  int                           max_body_size_;

 public:
  Server();
  Server& operator=(const Server &other)  = default;
  Server(const Server &other)             = default;
  ~Server()                               = default;

  const string&                 GetListen() const;
  void                          SetListen(const string &listen);
  const std::vector<string>&    GetServerNames() const;
  bool                          FindServerName(string &name);
  void                          AddServerName(const string &name);
  const std::vector<Location>&  GetLocations() const;
  const Location*               GetLocation(const string &uri) const;
  void                          AddLocation(const Location &new_loc);
  string                        GetErrorPage(int error_val) const;
  void                          SetErrorPage(int error_val, string &error_file);
  int                           GetMaxBodySize() const;
  void                          SetMaxBodySize(int max_body_size);
  void                          Print() const;
// если заданы то подтягиваем файлы, елси нет - генерим свои

  Response* CreateResponse(Request &request) const;

 private:
  static Response* ResponseFromGet(Request &request, const std::string &path,
                                   const Location *location);
  static Response* ResponseFromHead(Request &request, const std::string &path,
                                    const Location *location);
  static Response* ResponseFromPut(Request &request, const std::string &path,
                                   const Location *location);

  static std::map
  <std::string,
  std::function<Response*(Request&, const std::string&, const Location*)> >
                                                          response_from_methods;

  static Response    *GetResponseFromLocationIndex(const Location &location);
  const Location     *FindLocation(std::string path) const;
  static std::string JoinPath(const std::string &a, const std::string &b);
  static std::string GetRealRoot();
  static std::string FileToString(const char *filename);
  static std::string GetContentType(const std::string &filename);

  static std::map<std::string, std::string> content_types;

 public:
  class Exception: public std::exception {
   public:
    virtual const char* what() const throw();
  };
};
