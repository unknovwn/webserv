#include "request.hpp"
#include <utility>

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
    throw Request::Exception();
  return (*it).first;
}
const string &Request::Find_GetH_Opt(const string &key) const {
  auto it = headers_.find(key);
  if (it == headers_.end())
    throw Request::Exception();
  return (*it).second;
}
void Request::SetHeader(const string &name, const string &option) {
  auto it = headers_.find(name);
  if (it == headers_.end())
    headers_.insert(std::pair<string, string>(name, option));
  else if ((*it).first == name)
    (*it).second = option;
}
//============================== IP_PORT =======================================
const string &Request::GetIpPort() const {
  return ip_port_;
}
void Request::SetIpPort(const string &ip_port) {
  Request::ip_port_ = ip_port;
}
//========================== EXCEPTION =========================================
const char *Request::Exception::what() const throw() {
  return ("Request Exception\n");
}
//=========================== CLEAR ============================================
void Request::Clear() {
	ip_port_.clear();
	method_.clear();
	path_.clear();
	protocol_.clear();
	headers_.clear();
}
//void Print() const {
//  std::cout << "uri: " << uri_ << std::endl;
//  std::cout << "root: " << root_ << std::endl;
//  std::cout << "index: "
//  for (auto& index : index_) {
//    std::cout << index << " ";
//  }
//  std::cout << std::endl;
//  std::cout << "autoindex: " << autoindex_ << std::endl;
//  std::cout << "methods: ";
//  for (auto& method : methods_) {
//    std::cout << method << " ";
//  }
//  std::cout << std::endl;
//  std::cout << "upload_dir: " << upload_dir_ << std::endl;
//}
