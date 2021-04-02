#pragma once

#include <string>
#include <vector>

class Request{

  std::string               method_;
  std::string               path_;
  std::string               protocol_;
  std::vector<std::string>  headers_;
 public:

};