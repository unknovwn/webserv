#include <sys/stat.h>
#include <dirent.h>
#include <iostream>
#include <fstream>

bool   fileExists(const std::string &file) {
  struct stat buff;

  return (stat(file.c_str(), &buff) == 0);
}

void    print_dir(std::string &path, std::string &root) {
  DIR *dir;
  struct dirent *entry;


  dir = opendir(path.c_str());   // достаем из string - char *
  if (!dir) {
    std::cerr << "Cannot open this directory" << std::endl;
    return;
  }

  if (path.back() != '/')   // if u put PATH without end '/'
    path += '/';
  if (root.back() != '/')
    root += '/';
  std::ofstream out("autoindex.html");
  out << "<!DOCTYPE html>"                         << std::endl;
  out << "<html>"                                  << std::endl;
  out << "<title> Index of " << path << "</title>" << std::endl;
  out << "<body>"                                  << std::endl;
  out << "<div>"                                   << std::endl;
  out << "<h1>Index of ";

  size_t found = path.find(root);
  path.erase(found, root.length() - 1);
  out << path;
  out << "</h1>" << std::endl;
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

  std::string path_("/Users/bmicheal/Projects/webserv/src/autoindex/root/");
  std::string root_("/Users/bmicheal/Projects/webserv/src/autoindex/root");

  if (fileExists("index.php") == false)
    print_dir(path_, root_);
  else
    std::cout << "index.php Exists" << std::endl;
}
