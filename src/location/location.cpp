#include "location.hpp"

//=============================== URI ==========================================
const std::string& Location::GetUri() const {
  return uri_;
}
void Location::SetUri(const std::string &uri) {
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
const std::string& Location::GetRoot() const {
  return root_;
}
void Location::SetRoot(const std::string &root) {
  root_ = root;
}
//=============================== INDEX ========================================
const std::string& Location::GetIndex(size_t id) const {
  return index_[id];
}
void Location::SetIndex(const std::vector<std::string> &index) {
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
const std::string &Location::GetUploadDir() const {
  return upload_dir_;
}
void Location::SetUploadDir(const std::string &upload_dir) {
  upload_dir_ = upload_dir;
}