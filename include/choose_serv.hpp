#pragma once

#include <vector>
#include "server.hpp"
#include "request.hpp"

Server *choose_serv(Request &request, std::vector<Server> &servers) {
  std::vector <Server*> friendly;
  size_t num_of_matches = 0;

  for (auto &cur : servers) {
    if (request.GetIpPort() == cur.GetListen()) {
      ++num_of_matches;
      friendly.push_back(&cur);
    }
  }
  if (num_of_matches == 1)
    return friendly[0];
  else if (num_of_matches == 0)
    return nullptr;

  for (auto &cur : friendly) {
    if (request.Find_GetH_Opt("Host") == cur->GetServName())
      return cur;
  }
  return friendly[0];
}
