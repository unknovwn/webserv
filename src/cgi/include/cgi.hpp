//
// Created by Gueren Drive on 4/20/21.
//

#pragma once

#ifndef CGIREQUESTRESPONSE_CGI_HPP
#define CGIREQUESTRESPONSE_CGI_HPP

#include <cstdlib>
#include <map>
#include <string>

#include "request.hpp"
#include "response.hpp"

class Cgi {
 public:
  Cgi() = delete;
  explicit Cgi(std::string cgi_bin_path);

  // Operations
  Response CreateResponse(Request &request);

  [[nodiscard]] char **CreateArgs() const;
  char **CreateEnv(Request &request) const;

  // Utils
  template <class T>
  void Free2DMatrix(T **matrix) {
    T **begin = matrix;
    while (*matrix) {
      free(*matrix);
      ++matrix;
    }
    free(begin);
  }

  // Exception
  class ResourceError {
    const char *what() const noexcept;
  };

 private:
  std::string cgi_bin_path_;
};

#endif  // CGIREQUESTRESPONSE_CGI_HPP
