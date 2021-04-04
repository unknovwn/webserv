#include "context_location.hpp"

//=============================== URI ==========================================
const std::string& location::GetUri() const {
  return uri_;
}
void location::SetUri(const std::string &uri) {
  uri_ = uri;
}
//=============================== AUTO INDEX ===================================
bool location::GetAutoindex() const {
  return autoindex_;
}
void location::SetAutoindex(bool autoindex) {
  autoindex_ = autoindex;
}
//=============================== METHODS ======================================
const http_method& location::GetMethods(size_t id) const {
  return methods_[id];
}
void location::SetMethods(const std::vector<http_method> &methods) {
  methods_ = methods;
}
//=============================== ROOT =========================================
const std::string& location::GetRoot() const {
  return root_;
}
void location::SetRoot(const std::string &root) {
  root_ = root;
}
//=============================== INDEX ========================================
const std::string& location::GetIndex(size_t id) const {
  return index_[id];
}
void location::SetIndex(const std::vector<std::string> &index) {
  index_ = index;
}
//=============================== MAX SIZE =====================================
size_t location::GetMaxSize() const {
  return max_body_size_;
}
void location::SetMaxSize(const size_t new_size) {
  max_body_size_ = new_size;
}
//============================ UPLOAD DIR ======================================
const std::string &location::GetUploadDir() const {
  return upload_dir_;
}
void location::SetUploadDir(const std::string &upload_dir) {
  upload_dir_ = upload_dir;
}
