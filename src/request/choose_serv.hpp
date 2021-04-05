#pragma once

#include "../server/server.hpp"
#include "../request/request.hpp"
#include <algorithm>
#define it__ std::vector<Server>::iterator

Server *choose_serv(Request &request, std::vector<Server> servers) {
    std::vector <Server*> friendly;
    size_t num_of_matches = 0;

	num_of_matches = std::count_if(servers.begin(), servers.end(),
								[&request, &friendly](Server &it){
	  if (it.GetHost() == (*request.GetHeaders().find("Host")).second) {
		friendly.push_back(&it);
	    return true;
	  }
	  return false;
	});
	if (num_of_matches == 1)
	  return friendly[0];
	else {

	}


//	for (const auto &x : servers_) {
//		if (x.GetHost() == *(request.GetHeaders().find("server_name")))
//	}
}

