#pragma once

#define map__ std::map<std::string, std::string>
#include <string>
#include <map>

using string = std::string;

class Request{

  string                 method_;
  string                 path_;
  string                 protocol_;
  map__                  headers_;
 public:
  const std::string&     GetMethod() const;
  void                   SetMethod(const std::string &method);
  const std::string&     GetPath() const;
  void                   SetPath(const std::string &path);
  const std::string&     GetAProtocol() const;
  void                   SetAProtocol(const std::string &a_protocol);
  map__&                 GetHeaders();
//  void                   SetHeaders(const map__ &headers);
};