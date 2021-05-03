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

#define BUFFER_SIZE 4096
#define MAX_CLIENTS 1024
#define CLIENT_LIFETIME 60

void recieve(std::map<int,
    std::pair<std::string, struct sockaddr_in> >& sock,
    [[maybe_unused]] const std::vector<Server>& servers) {
  std::ostringstream ss;
  std::ostringstream body;
  int  count;
  int  select_ret;
  char buffer[BUFFER_SIZE];
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
          std::cout << "Client " << client_sock[i]
            << " disconnected" << std::endl;
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
            clients[connected_sock] = Client(CLIENT_LIFETIME, address.first);
            std::cout << "Client " << client_sock[i] << " connected to "
              << address.first << std::endl;
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
          exit(EXIT_FAILURE);
        }
        if (count == 0 || (count == 1 && buffer[0] == 4)) {
          std::cout << "Client " << client_sock[i]
            << " disconnected" << std::endl;
          close(client_sock[i]);
          client_sock[i] = 0;
          clients.erase(client_sock[i]);
        } else {
          buffer[count] = '\0';
          std::cout << buffer;
          Client& client = clients[client_sock[i]];
          client.ResetTimer();

          Request* request;
          try {
            /* std::cout << buffer; */
            do {
              request = client.request_parser_.ParseRequest(buffer);
              if (request) {
                std::string response_str("Request parsed\r\n");
                /* Server& server = find_server(client.get_address()); */
                /* Response response = server.CreateResponse(*request); */
                /* std::string response_str = response.ToString(); */
                send(client_sock[i], response_str.c_str(),
                    response_str.length(), 0);
                delete request;
              }
            } while (!client.request_parser_.Empty());
          } catch (RequestParser::BadRequest& e) {
            std::string response_str("Bad Request\r\n");
            send(client_sock[i], response_str.c_str(),
                response_str.length(), 0);
            close(client_sock[i]);
            client_sock[i] = 0;
            clients.erase(client_sock[i]);
            /* Server& server = find_server(client.get_address()); */
            /* Response response = server.CreateBadRequest(); */
            /* std::string response_str = response.ToString(); */
          }
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
      config = "server.conf";
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
