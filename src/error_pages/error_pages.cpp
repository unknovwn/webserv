#include <iostream>
#include <sstream>
#include <map>
#include <vector>
#include <unordered_map>

std::string    generate_error_page(int error_code) {
  std::ostringstream out;
  static const std::map<int, std::string> my = {
  {400, "Bad Request"},
  {403, "Forbidden"},
  {404, "Not Found"},
  {405, "Method Not Allowed"},
  {410, "Gone"},
  {413, "Payload Too Large"},
  {500, "Internal Server Error"}};

  const auto i = my.find(error_code);
  if (i == my.end()) {
    return "";
  }
  out << "<!DOCTYPE html>"                              << std::endl;
  out << "<html>"                                       << std::endl;
  out << "<title>" << (*i).first << "</title>"          << std::endl;
  out << "<body>"                                       << std::endl;
  out << "<div>"                                        << std::endl;
  out << "<h1 align=\"center\">";
  out << (*i).first << " "  << (*i).second;
  out << "</h1>"                                        << std::endl;
  out << "<hr>"                                         << std::endl;
  out << "</div>"                                       << std::endl;
  out << "</body>"                                      << std::endl;
  out << "</html>"                                      << std::endl;
  return out.str();
}
