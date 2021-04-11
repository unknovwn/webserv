#include <cstring>
#include <string>
#include <vector>

bool contains_only_digits(const std::string& str) {
  return (str.find_first_not_of("0123456789") == std::string::npos);
}

std::vector<std::string> split(const std::string& str,
    const std::string& sep) {
  std::vector<std::string> result;
  char*                    token;
  char*                    saveptr;
  char*                    ch_str = new char[str.length() + 1];

  strcpy(ch_str, str.c_str());
  token = strtok_r(ch_str, sep.c_str(), &saveptr);
  while (token != NULL) {
    result.push_back(std::string(token));
    token = strtok_r(NULL, sep.c_str(), &saveptr);
  }
  delete[] ch_str;
  return result;
}
