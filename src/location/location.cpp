#include "location.hpp"

//=============================== URI ==========================================
const string& Location::GetUri() const {
  return uri_;
}
void Location::SetUri(const string &uri) {
  uri_ = uri;
}
//=============================== AUTO INDEX ===================================
bool Location::GetAutoindex() const {
  return autoindex_;
}
void Location::SetAutoindex(bool autoindex) {
  autoindex_ = autoindex;
}
//=============================== METHODS ======================================
const std::vector<string>& Location::GetMethods() const {
  return methods_;
}

void Location::AddMethod(const string& method) {
  methods_.push_back(method);
}
//=============================== ROOT =========================================
const string& Location::GetRoot() const {
  return root_;
}
void Location::SetRoot(const string &root) {
  root_ = root;
}
//=============================== INDEX ========================================
const std::vector<string>& Location::GetIndex() const {
  return index_;
}
void Location::AddIndex(const string& index) {
  index_.push_back(index);
}
//============================ UPLOAD DIR ======================================
const string &Location::GetUploadDir() const {
  return upload_dir_;
}
void Location::SetUploadDir(const string& upload_dir) {
  upload_dir_ = upload_dir;
}
//============================ CGI EXTENSION ===================================
const string& Location::GetCgiExtension() const {
  return cgi_extension_;
}
void Location::SetCgiExtension(const string& cgi_extension) {
  cgi_extension_ = cgi_extension;
}
//============================= CGI PATH =======================================
const string& Location::GetCgiPath() const {
  return cgi_path_;
}
void Location::SetCgiPath(const string& cgi_path) {
  cgi_path_ = cgi_path;
}
//============================== MAX BODY SIZE =================================
size_t Location::GetMaxBodySize() const {
  return this->max_body_size_;
}
void Location::SetMaxBodySize(size_t max_body_size) {
  this->max_body_size_ = max_body_size;
}
//========================== EXCEPTION =========================================
const char *Location::Exception::what() const throw() {
  return ("Location context Exception\n");
}
//=========================== '==' =============================================
bool Location::operator==(const Location &other) const {
  return (
  uri_ == other.uri_ &&
  root_ == other.root_ &&
  index_ == other.index_ &&
  autoindex_ == other.autoindex_ &&
  methods_ == other.methods_ &&
  upload_dir_ == other.upload_dir_ &&
  max_body_size_ == other.max_body_size_);
}

