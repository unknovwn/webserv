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
const http_method& Location::GetMethods(size_t id) const {
  return methods_[id];
}
void Location::SetMethods(const std::vector<http_method> &methods) {
  methods_ = methods;
}
//=============================== ROOT =========================================
const string& Location::GetRoot() const {
  return root_;
}
void Location::SetRoot(const string &root) {
  root_ = root;
}
//=============================== INDEX ========================================
const string& Location::GetIndex(size_t id) const {
  return index_[id];
}
void Location::SetIndex(const std::vector<string> &index) {
  index_ = index;
}
//=============================== MAX SIZE =====================================
size_t Location::GetMaxSize() const {
  return max_body_size_;
}
void Location::SetMaxSize(const size_t new_size) {
  max_body_size_ = new_size;
}
//============================ UPLOAD DIR ======================================
const string &Location::GetUploadDir() const {
  return upload_dir_;
}
void Location::SetUploadDir(const string &upload_dir) {
  upload_dir_ = upload_dir;
}
//========================== EXCEPTION =========================================
const char *Location::Exception::what() const throw() {
  return ("Location context Exception\n");
}
//=========================== '==' =============================================
bool Location::operator==(const Location &other) const {
  return (
  	Location::uri_ == other.uri_ &&
  	Location::root_ == other.root_ &&
  	Location::index_ == other.index_ &&
  	Location::autoindex_ == other.autoindex_ &&
  	Location::methods_ == other.methods_ &&
  	Location::max_body_size_ == other.max_body_size_ &&
  	Location::upload_dir_ == other.upload_dir_
  	);
}
