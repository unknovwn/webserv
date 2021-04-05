#pragma once

#include <iostream>
#include <string>
#include <vector>

typedef enum {
  GET,
  HEAD,
  POST,
  PUT
} http_method;

class Location {
  std::string                     uri_;
  std::string                     root_;
  std::vector<std::string>        index_;
  bool                            autoindex_;
  std::vector<http_method>        methods_; //allowed methods
  size_t                          max_body_size_;
  std::string                     upload_dir_;

 public:
  Location()                                  = default;
  Location& operator=(const Location &other)  = default;
  Location(const Location &other)             = default;
  ~Location()                                 = default;

  const std::string&              GetUri() const;
  void                            SetUri(const std::string &uri);
  const std::string&              GetRoot() const;
  void                            SetRoot(const std::string &root);
  const std::string&              GetIndex(size_t id) const;
  void                            SetIndex
	                                 (const std::vector<std::string> &index);
  bool                            GetAutoindex() const;
  void                            SetAutoindex(bool autoindex);
  const http_method&              GetMethods(size_t id) const;
  void                            SetMethods(const std::vector<http_method>
                                    &methods);
  size_t                          GetMaxSize() const;
  void                            SetMaxSize(const size_t new_size);
  const std::string&              GetUploadDir() const;
  void                            SetUploadDir(const std::string &upload_dir);

  class Exception: public std::exception { // пока не нужно, оставлю
   public:
	virtual const char* what() const throw();
  };

  bool operator==(const Location &other) const;
};//end of class
