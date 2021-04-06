#include <iostream>
#include <fstream>
#include <map>

#define T '\t'

void    generate_error_page(size_t error_code) {
  std::ofstream out ("error.html");
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
    out.close();
    std::remove("error.html");
    return;
  }
  out << "<!DOCTYPE html>"                                 << std::endl;
  out << "<html>"                                          << std::endl;
  out << T << "<title>"        << (*i).first << "</title>" << std::endl;
  out << T << "<body>"                                     << std::endl;
  out << T << T << "<div>"                                 << std::endl;
  out << T << T << T << "<h1 align=\"center\">";
  out << (*i).first << " "  << (*i).second;
  out << "</h1>"                                           << std::endl;
  out << T << T << T << "<hr>"                             << std::endl;
  out << T << T << "</div>"                                << std::endl;
  out << T << "</body>"                                    << std::endl;
  out << "</html>"                                         << std::endl;
  out.close();
}