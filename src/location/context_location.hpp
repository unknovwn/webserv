#pragma once

#include <iostream>
#include <string>
#include <vector>

typedef enum
{
  GET,
  HEAD,
  POST,
  PUT,
  DELETE,
  CONNECT,
  OPTIONS,
  TRACE,
  PATCH
} http_method;

class location {

  std::string               uri_;
  std::string               root_;
  bool                      autoindex_;
  std::vector<http_method>  methods_;

 public:

  const std::string&              GetUri() const;
  void                            SetUri(const std::string &uri);
  bool                            GetAutoindex() const;
  void                            SetAutoindex(bool autoindex);
  const std::vector<http_method>& GetMethods() const;
  void                            SetMethods(const std::vector<http_method> &methods);
  const std::string&              GetRoot() const;
  void                            SetRoot(const std::string &root);
};
