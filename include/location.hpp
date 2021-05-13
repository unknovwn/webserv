#pragma once

#include <iostream>
#include <string>
#include <vector>

using string = std::string;

class Location {
  string                          uri_;
  string                          root_;
  std::vector<string>             index_;
  bool                            autoindex_;
  std::vector<string>             methods_;   // allowed methods
  string                          upload_dir_;
  string                          cgi_extension_;
  string                          cgi_path_;
  size_t                          max_body_size_ = 1048576;

 public:
  Location()                                  = default;
  Location& operator=(const Location &other)  = default;
  Location(const Location &other)             = default;
  ~Location()                                 = default;

  const string&                   GetUri() const;
  void                            SetUri(const string &uri);
  const string&                   GetRoot() const;
  void                            SetRoot(const string &root);
  const std::vector<string>&      GetIndex() const;
  void                            AddIndex(const string& index);
  bool                            GetAutoindex() const;
  void                            SetAutoindex(bool autoindex);
  const std::vector<string>&      GetMethods() const;
  void                            AddMethod(const string& method);
  const string&                   GetUploadDir() const;
  void                            SetUploadDir(const string& upload_dir);
  const string&                   GetCgiExtension() const;
  void                            SetCgiExtension(const string& cgi_extension);
  const string&                   GetCgiPath() const;
  void                            SetCgiPath(const string& cgi_path);
  void                            SetMaxBodySize(size_t max_body_size);
  size_t                          GetMaxBodySize() const;

  class Exception: public std::exception {   // пока не нужно, оставлю
   public:
    virtual const char* what() const throw();
  };

  bool operator==(const Location &other) const;
};   // end of class
