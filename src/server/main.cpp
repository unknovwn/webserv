#include "server.hpp"
#include "../request/choose_serv.hpp"


void  create_servs(std::vector<Server> &servers_) {
  servers_[0].SetListen("192.168.8.1:80");
  servers_[1].SetListen("192.168.8.1:70");
  servers_[2].SetListen("192.168.8.1:80");
  servers_[3].SetListen("192.168.8.1:80");

  servers_[0].SetServName("yandex.ru");
  servers_[1].SetServName("gmail.com");
  servers_[2].SetServName("pornhub.ru");
  servers_[3].SetServName("pornhub.org");
}

int   main() {
  Server *matched_serv = nullptr;
  std::vector<Server> servers_(4);
  Request request;

  create_servs(servers_);

  request.SetIpPort("192.168.8.1:80");
  request.SetHeader("Host", "pornhub.net");
  matched_serv = choose_serv(request, servers_);

  if (matched_serv == NULL) {
    std::cout << "Not Found" << std::endl;
  } else {
    std::cout << "Finded Port: " << matched_serv->GetListen();
    std::cout << "  Finded Host: " << matched_serv->GetServName() << std::endl;
  }

  return 0;
}
