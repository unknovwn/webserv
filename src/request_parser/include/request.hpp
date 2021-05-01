#pragma once

#define map__ std::map<std::string, std::string>
#include <string>
#include <map>

using string = std::string;

class Request{
  string                 ip_port_;
  string                 method_;
  string                 path_;
  string                 protocol_;
  string                 body_;
  map__                  headers_;

 public:
  Request()                                 = default;
  Request& operator=(const Request &other)  = default;
  Request(const Request &other)             = default;
  ~Request()                                = default;

  const string&          GetMethod() const;
  void                   SetMethod(const string &method);
  const string&          GetPath() const;
  void                   SetPath(const string &path);
  const string&          GetAProtocol() const;
  void                   SetAProtocol(const string &a_protocol);
  map__&                 GetHeaders();   // доступ к мапе ( на всякий случай)
  void                   AddToBody(const std::string& content);
  const std::string&     GetBody() const;
  const string&          Find_GetH_Name(const string &name) const;
  const string&          Find_GetH_Opt(const string &option) const;
  void                   SetHeader(const string &name, const string &option);
  const string&          GetIpPort() const;
  void                   SetIpPort(const string &ip_port);
  void                   Clear();

  class Exception: public std::exception {
   public:
    virtual const char* what() const throw();
  };
};
