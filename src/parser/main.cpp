#include <sstream>
#include <fstream>
#include <iostream>

enum state {
  root,
  server
};

int main() {
  /* std::ifstream config_file("webserv.conf"); */
  /* if (!config_file.is_open()) { */
  /*  std::cerr << "Error opening file" << std::endl; */
  /*  exit(EXIT_FAILURE); */
  /* } */
  /* std::string line; */
  /* std::string word; */
  /* size_t current_index; */
  /* state current_state = state.root; */
  /* while (true) { */
  /*  std::getline(config_file, line); */
  /*  if (!config_file) { */
  /*    break; */
  /*  } */
  /*  current_index = line.find("#", 0); */
  /*  if (current_index != std::string::npos) { */
  /*    line.resize(current_index); */
  /*  } */
  /*  std::istringstream iss(line); */
  /*  while (iss >> word) { */
  /*    std::cout << word << std::endl; */
  /*    switch (current_state) { */
  /*      case state.root: { */
  /*        if (word != "{") */
  /*      } */
  /*    } */
  /*  } */
  /* } */
  return 0;
}
