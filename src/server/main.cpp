#include "server.hpp"
#include "../request/choose_serv.hpp"


int main()
{
  Server *matched_serv;
  std::vector<Server> servers_(4);

  servers_[0].SetListen("192.168.8.1:80");
  servers_[0].SetListen("192.168.8.1:70");
  servers_[1].SetListen("192.168.8.1:80");
  servers_[2].SetListen("192.168.8.1:80");

  Request request;
  request.SetIpPort("192.168.1:70");
  request.SetHeader("Host", "pornhub.com");
  choose_serv(request, servers_);
  return 0;
}