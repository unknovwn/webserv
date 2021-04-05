#pragma once

#include <iostream>
#include <string>
#include <vector>

using string = std::string;

typedef enum {
  GET,
  HEAD,
  POST,
  PUT
} http_method;

class Location {
  string                     uri_;
  string                     root_;
  std::vector<string>        index_;
  bool                            autoindex_;
  std::vector<http_method>        methods_; //allowed methods
  size_t                          max_body_size_;
  string                     upload_dir_;

 public:
  Location()                                  = default;
  Location& operator=(const Location &other)  = default;
  Location(const Location &other)             = default;
  ~Location()                                 = default;

  const string&                   GetUri() const;
  void                            SetUri(const string &uri);
  const string&                   GetRoot() const;
  void                            SetRoot(const string &root);
  const string&                   GetIndex(size_t id) const;
  void                            SetIndex
	                                 (const std::vector<string> &index);
  bool                            GetAutoindex() const;
  void                            SetAutoindex(bool autoindex);
  const http_method&              GetMethods(size_t id) const;
  void                            SetMethods(const std::vector<http_method>
                                    &methods);
  size_t                          GetMaxSize() const;
  void                            SetMaxSize(const size_t new_size);
  const string&                   GetUploadDir() const;
  void                            SetUploadDir(const string &upload_dir);

  class Exception: public std::exception { // пока не нужно, оставлю
   public:
	virtual const char* what() const throw();
  };

  bool operator==(const Location &other) const;
};//end of class
