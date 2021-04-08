#include <iostream>
#include <fstream>
#include <map>
#include <vector>
#include <unordered_map>

void    generate_error_page(size_t error_code) {
  std::ofstream out("error.html");
  static const std::map<int, std::string> my = {
  {400, "Bad Request"},
  {403, "Forbidden"},
  {404, "Not Found"},
  {405, "Method Not Allowed"},   // GET & HEAD cant be allowed
  {410, "Gone"},
  {413, "Payload Too Large"},
  {500, "Internal Server Error"}};

  const auto i = my.find(error_code);
  if (i == my.end()) {
    out.close();
    std::remove("error.html");
    return;
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
  out.close();
}

int main() {
//  generate_error_page(400);

  int target = 6;

  std::vector<int> vec;
  vec.push_back(3);
  vec.push_back(2);
  vec.push_back(7);
  vec.push_back(1);
  vec.push_back(4);

  // map : 0 - 0 ; 1 - 3; 2 - 5; 3 - 12; 4 - 13; 5 - 17;
  std::unordered_map<int, int> map_;

//  for (int i = 0; i < vec.size(); i++) {
//	map_.insert(std::pair<int, int>(vec[i], i));
//  }
map_[0] = 0;
  for (int i = 1; i < vec.size(); i++) {
    map_[i] = map_[i - 1] + vec[i - 1];
	//map_.insert(std::pair<int, int>(i, i));
  }

//  for (int i = 0; i < nums.length; i++) {
//	int complement = target - nums[i];
//	if (map.containsKey(complement) && map.get(complement) != i) {
//	  return new int[] { i, map.get(complement) };
//	}
//  }
}
