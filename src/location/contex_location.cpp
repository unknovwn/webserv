#include "context_location.hpp"

const std::string& location::GetUri() const {
  return uri_;
}
void location::SetUri(const std::string &uri) {
  uri_ = uri;
}

bool location::GetAutoindex() const {
  return autoindex_;
}
void location::SetAutoindex(bool autoindex) {
  autoindex_ = autoindex;
}

const http_method& location::GetMethods(size_t id) const {
  return methods_[id];
}
void location::SetMethods (const std::vector<http_method> &methods) {
  methods_ = methods;
}

const std::string& location::GetRoot() const {
  return root_;
}
void location::SetRoot(const std::string &root) {
  root_ = root;
}

const std::string& location::GetIndex(size_t id) const {
  return index_[id];
}
void location::SetIndex(const std::vector<std::string> &index) {
  index_ = index;
}

size_t location::GetMaxSize() const {
  return max_body_size_;
}
void location::SetMaxSize(const size_t new_size) {
  max_body_size_ = new_size;
}
