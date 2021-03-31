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

class location {
  std::string               uri_;
  std::string               root_;
  std::vector<std::string>  index_;
  bool                      autoindex_;
  std::vector<http_method>  methods_;
  size_t                    max_body_size_;

 public:
  const std::string&              GetUri() const;
  void                            SetUri(const std::string &uri);
  bool                            GetAutoindex() const;
  void                            SetAutoindex(bool autoindex);
  const std::vector<http_method>& GetMethods() const;
  void                            SetMethods(const std::vector<http_method>
                                    &methods);
  const std::string&              GetRoot() const;
  void                            SetRoot(const std::string &root);
  const std::vector<std::string>& GetIndex() const;
  void                            SetIndex(
  	                                const std::vector<std::string> &index);
  size_t                          GetMaxSize() const;
  void                            SetMaxSize(const size_t new_size);
};
