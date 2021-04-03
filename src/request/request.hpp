#pragma once

#include <string>
#include <map>

class Request{

  std::string                        method_;
  std::string                         path_;
  std::string                         protocol_;
  std::map<std::string, std::string>  headers_;
 public:

};