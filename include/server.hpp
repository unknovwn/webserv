#pragma once

#include <memory>
#include <string>
#include <vector>
#include <map>
#include <utility>
#include <functional>

#include "location.hpp"


class Request;
class Response;

class Server {
 private:
  std::string                listen_;
  std::vector<std::string>   server_names_;
  std::vector<Location>      routes_;
  std::map<int, std::string> error_pages_;

 public:
  Server();

  // Setters/Getters
  void                            SetListen(const std::string &listen);
  const std::string&              GetListen() const;
  const std::vector<std::string>  &GetServerNames() const;
  const std::vector<Location>     &GetLocations() const;
  void                            SetErrorPage(int error_val,
                                               std::string &error_file);
  std::string                     GetErrorPage(int error_val) const;

  // Modifiers
  void                          AddServerName(const std::string &name);
  void                          AddLocation(const Location &new_loc);


// DELETED
//  const Location*               GetLocation(const std::string &uri) const;
//  bool                          FindServerName(std::string &name);
//  void                          Print() const;

  // Operations
  Response*        CreateResponse(Request &request) const;
  Response*        CreateBadRequestResponse() const;
  std::string      GetErrorPageBody(int status_code) const;

 private:
  // Response from methods
  Response* ResponseFromGet(Request &request, const std::string &path,
      const Location *location) const;
  Response* ResponseFromHead(Request &request, const std::string &path,
      const Location *location) const;
  Response* ResponseFromPut(Request &request, const std::string &path,
      const Location *location) const;
  Response* ResponseFromPost(Request &request, const std::string &path,
      const Location *location) const;

  // Response Utils
  static Response    *ResponseFromLocationIndex(const Location &location,
                                                const std::string &path);
  static Response    *ResponseFromAutoIndex(std::string absolute_path,
                                            std::string relative_path);
  static Response    *Response404();

  // Element Access
  const Location *FindLocation(const std::string &path) const;

  // Info
  static bool MethodIsAllowed(Request &request, const Location &location);

  // Utils
  static std::string GetAbsolutePath();
  static std::string GetAbsolutePath(const std::string &relative_path);
  static std::string GetAbsolutePathFromLocation(Request &request,
                                                 const Location &location);
  static std::string JoinPath(const std::string &a, const std::string &b);
  static std::string FileToString(const std::string &filename);
  static std::string GetContentType(const std::string &filename);
  static std::string CropFromLastSymbol(const std::string &str, char c);
  static int         MakeDir(const char *path);
  static bool        IsDir(const std::string &path);
  static bool        FileExist(const std::string &path);

  static std::map<std::string, std::string> content_types;

 public:
  // Exceptions
  class Exception: public std::exception {
   public:
    virtual const char* what() const throw();
  };
  class FileDoesNotExist {
   public:
    FileDoesNotExist() = delete;
    FileDoesNotExist(const std::string &file_name);

    virtual const char *what() const noexcept;

   private:
    std::string file_name_;
  };
};
