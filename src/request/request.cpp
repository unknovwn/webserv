#include "request.hpp"

//============================== METHOD ========================================
const string &Request::GetMethod() const {
  return method_;
}
void Request::SetMethod(const string &method) {
  method_ = method;
}
//============================== PATH ==========================================
const string &Request::GetPath() const {
  return path_;
}
void Request::SetPath(const string &path) {
  path_ = path;
}
//============================== PROTOCOL ======================================
const string &Request::GetAProtocol() const {
  return protocol_;
}
void Request::SetAProtocol(const string &a_protocol) {
  protocol_ = a_protocol;
}
//============================== HEADERS =======================================
std::map<string, string> &Request::GetHeaders() {
  return headers_;
}
const string &Request::Find_GetH_Name(const string &key) const {
   auto it = headers_.find(key);
   if (it == headers_.end())
     return NULL;
  return (*it).first;
}
const string &Request::Find_GetH_Opt(const string &key) const {
  auto it = headers_.find(key);
  if (it == headers_.end())
	return NULL;
  return (*it).second;
}
void Request::SetHeader(const string &name, const string &option) {
  headers_.insert(std::pair<string, string>(name, option));
}

//============================== IP_PORT =======================================
const string &Request::GetIpPort() const {
  return ip_port_;
}
void Request::SetIpPort(const string &ip_port) {
  Request::ip_port_ = ip_port;
}
