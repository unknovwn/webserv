#include "request.hpp"

//============================== METHOD ========================================
const std::string &Request::GetMethod() const {
  return method_;
}
void Request::SetMethod(const std::string &method) {
  method_ = method;
}
//============================== PATH ==========================================
const std::string &Request::GetPath() const {
  return path_;
}
void Request::SetPath(const std::string &path) {
  path_ = path;
}
//============================== PROTOCOL ======================================
const std::string &Request::GetAProtocol() const {
  return protocol_;
}
void Request::SetAProtocol(const std::string &a_protocol) {
  protocol_ = a_protocol;
}
//============================== HEADERS =======================================
std::map<std::string, std::string> &Request::GetHeaders() {
  return headers_;
}
const std::string &Request::Find_GetH_Name(const std::string &name) const {
   auto it = headers_.find(name);
   if (it == headers_.end())
     return NULL;
  return (*it).first;
}
const string &Request::Find_GetH_Opt(const string &option) const {
  auto it = headers_.find(option);
  if (it == headers_.end())
	return NULL;
  return (*it).second;
}
void Request::SetHeader(const string &name, const string &option) {
  headers_.insert(std::pair<string, string>(name, option));
}

//============================== IP_PORT =======================================
const string &Request::GetIpPort() const {
  return ip_port;
}
void Request::SetIpPort(const string &ip_port) {
  Request::ip_port = ip_port;
}
