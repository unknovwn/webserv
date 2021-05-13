#include <sys/socket.h>
#include <sys/errno.h>
#include <sys/select.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/time.h>
#include <cstring>
#include <sstream>
#include <iostream>
#include <map>
#include <fstream>
#include <set>
#include <utility>
#include "lexer.hpp"
#include "client.hpp"
#include "config_parser.hpp"
#include "utils_string.hpp"
#include "cgi.hpp"
#include "request.hpp"

#define BUFFER_SIZE 32768
#define MAX_CLIENTS 1000
#define CLIENT_LIFETIME 300

const Server* find_server(const std::vector<Server>& servers,
                          const Request& request) {
  std::vector<const Server*> friendly;
  size_t num_of_matches = 0;

  for (const auto& cur : servers) {
    if (request.GetIpPort() == cur.GetListen()) {
      ++num_of_matches;
      friendly.push_back(&cur);
    }
  }

  for (const auto& cur : friendly) {
    auto serv_names = cur->GetServerNames();
    if (std::find(serv_names.begin(), serv_names.end(),
          request.Find_GetH_Opt("Host")) != serv_names.end())
      return cur;
  }
  return friendly.at(0);
}

void recieve(std::map<int,
    std::pair<std::string, struct sockaddr_in> >& sock,
    const std::vector<Server>& servers) {
  int  count;
  int  select_ret;
  char buffer[BUFFER_SIZE + 1];
  int  client_sock[MAX_CLIENTS];
  fd_set rfds;
  std::map<int, Client> clients;
  int  max_sd;
  int  sd;
  int  connected_sock;
  int  addrlen = sizeof(struct sockaddr_in);
  struct timeval timeout;

  timeout.tv_sec = 5;
  timeout.tv_usec = 0;

  for (int i = 0; i < MAX_CLIENTS; i++) {
    client_sock[i] = 0;
  }
  while (true) {
    max_sd = 0;
    FD_ZERO(&rfds);
    for (const auto& entry : sock) {
      if (entry.first > max_sd) {
        max_sd = entry.first;
      }
      FD_SET(entry.first, &rfds);
    }
    for (int i = 0; i < MAX_CLIENTS; i++) {
      sd = client_sock[i];
      if (sd > 0) {
        if (!clients[sd].Alive()) {
          close(sd);
          clients.erase(sd);
          client_sock[i] = 0;
          continue;
        }
        FD_SET(sd, &rfds);
      }
      if (sd > max_sd) {
        max_sd = sd;
      }
    }

    select_ret = select(max_sd + 1, &rfds, NULL, NULL, &timeout);

    if (select_ret == -1) {
      std::cerr << "Select error" << std::endl;
      exit(EXIT_FAILURE);
    }

    if (select_ret == 0) {
      continue;
    }

    for (auto& [sockfd, address] : sock) {
      if (FD_ISSET(sockfd, &rfds)) {
        connected_sock = accept(sockfd,
            reinterpret_cast<struct sockaddr*>(&(address.second)),
            reinterpret_cast<socklen_t*>(&addrlen));
        if (connected_sock == -1) {
          std::cerr << "Accept error" << std::endl;
          exit(EXIT_FAILURE);
        }

        for (int i = 0; i < MAX_CLIENTS; i++) {
          if (client_sock[i] == 0) {
            client_sock[i] = connected_sock;
            clients[connected_sock]
              = Client(CLIENT_LIFETIME, address.first, sockfd);
            break;
          }
        }
      }
    }

    for (int i = 0; i < MAX_CLIENTS; i++) {
      if (FD_ISSET(client_sock[i], &rfds)) {
        count = recv(client_sock[i], buffer, BUFFER_SIZE, 0);
        if (count == -1) {
          std::cerr << "Recv error" << std::endl;
          continue;
        }
          buffer[count] = '\0';
          Client& client = clients[client_sock[i]];
          client.ResetTimer();

          Request* request;
          try {
            do {
              request = client.request_parser_.ParseRequest(buffer);
              buffer[0] = '\0';
              if (request) {
                request->SetIpPort(sock[client.get_sockfd()].first);
                const Server* server = find_server(servers, *request);
                Response* response = server->CreateResponse(*request);
                std::string response_str;
                if (!response->GetCgiResponse().empty()) {
                  response_str = response->GetCgiResponse();
                } else {
                  response_str = response->ToString();
                }
                send(client_sock[i], response_str.c_str(),
                    response_str.length(), 0);
                delete request;
                delete response;
              }
            } while (!client.request_parser_.Empty());
          } catch (RequestParser::BadRequest& e) {
            std::unique_ptr<Response> response(servers[0]
                .CreateBadRequestResponse());
            std::string response_str = response->ToString();
            send(client_sock[i], response_str.c_str(),
                response_str.length(), 0);
            close(client_sock[i]);
            client_sock[i] = 0;
            clients.erase(client_sock[i]);
          }
      }
    }
  }
}

int main(int argc, char** argv) {
  std::map<int, std::pair<std::string, struct sockaddr_in> > sock;
  std::vector<Server>                                        servers;
  std::set<std::string>                                      addresses;
  std::string                                                config;

  switch (argc) {
    case 1:
      config = "configs/serv.conf";
      break;
    case 2:
      config = argv[1];
      break;
    default:
      std::cerr << "Wrong number of arguments" << std::endl;
      return 0;
  }

  try {
    Lexer lexer(config);
    servers = ConfigParser::GetInstance().ParseConfig(lexer.get_lexeme());
  } catch (Lexer::FileError& e) {
    std::cerr << e.what() << std::endl;
    return 0;
  } catch (ConfigParser::ConfigError& e) {
    std::cerr << config << ":" << e.GetLine() << ": "
      << "\x1B[31merror: \033[0m" << e.what() << std::endl;
    return 0;
  }

  for (const auto& server : servers) {
    addresses.insert(server.GetListen());
  }

  for (const auto& address : addresses) {
    std::vector<std::string> tokens = split(address, ":");
    struct sockaddr_in       sockaddr;
    int                      opt = 1;
    int                      sockfd;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
      std::cerr << "Cannot create socket" << std::endl;
      exit(EXIT_FAILURE);
    }

    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) {
      std::cerr << "Setsockopt error" << std::endl;
      exit(EXIT_FAILURE);
    }
    sockaddr.sin_family = AF_INET;
    sockaddr.sin_addr.s_addr = inet_addr(tokens[0].c_str());
    sockaddr.sin_port = htons(std::stoi(tokens[1]));

    if (bind(sockfd, reinterpret_cast<struct sockaddr*>(&sockaddr),
          sizeof(sockaddr)) == -1) {
      std::cerr << "Bind error" << std::endl;
      std::cerr << address << std::endl;
      std::cerr << strerror(errno) << std::endl;
      exit(EXIT_FAILURE);
    }
    if (listen(sockfd, MAX_CLIENTS) == -1) {
      std::cerr << "Listen error" << std::endl;
      exit(EXIT_FAILURE);
    }
    sock[sockfd] = std::make_pair(address, sockaddr);
  }

  recieve(sock, servers);

  return 0;
}
