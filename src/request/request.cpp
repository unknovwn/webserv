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
//void Request::SetHeaders(const std::map<std::string, std::string> &headers) {
//  headers_ = headers;
//}
