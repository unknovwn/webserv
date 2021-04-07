#include <dirent.h>
#include <iostream>
#include <fstream>
#include <sys/stat.h>

bool   fileExists(const std::string file) {
  struct stat buff;

  return (stat(file.c_str(), &buff) == 0);
}

void    print_dir(const std::string &path) {
  DIR *dir;
  struct dirent *entry;

  dir = opendir(path.c_str());   // достаем из string - char *
  if (!dir) {
    std::cerr << "Cannot open this directory" << std::endl;
    return;
  }

  std::ofstream out ("autoindex.html");
  out << "<!DOCTYPE html>"                         << std::endl;
  out << "<html>"                                  << std::endl;
  out << "<title> Index of " << path << "</title>" << std::endl;
  out << "<body>"                                  << std::endl;
  out << "<div>"                                   << std::endl;
  out << "<h1>Index of /</h1>"                     << std::endl;
  out << "<hr>"                                    << std::endl;
  while ((entry = readdir(dir)) != NULL) {
    out << "<p><a href=\"" << path + "/" + entry->d_name << "\">";
    out << entry->d_name;
    if (entry->d_type & DT_DIR)
      out << "/";
    out << "</a></p>" << std::endl;
  }
  out << "</div>"                                   << std::endl;
  out << "</body>"                                  << std::endl;
  out << "</html>"                                  << std::endl;
  out.close();

  closedir(dir);
}

int main() {
  if (fileExists("index.php") == false)
    print_dir("/Users/bmicheal/Projects/webserv/src/autoindex");
  else
    std::cout << "index.php Exists" << std::endl;
}