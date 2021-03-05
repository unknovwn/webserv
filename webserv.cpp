#include <sys/socket.h>
#include <sys/select.h>
#include <sys/errno.h>
#include <cstring>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sstream>
#include <iostream>

#define BUFFER_SIZE 1024
#define PORT 8080
#define MAX_CLIENTS 6

void recieve(int sockfd, struct sockaddr_in address, int addrlen) {
	std::ostringstream ss;
	std::ostringstream body;
	int count;
	int select_ret;
	char buffer[BUFFER_SIZE];
	int client_sock[MAX_CLIENTS];
	fd_set rfds;
	int max_sd;
	int sd;
	int connected_sock;

	for (int i = 0; i < MAX_CLIENTS; i++) {
		client_sock[i] = 0;
	}
	while (true) {
		max_sd = sockfd;
		FD_ZERO(&rfds);
		FD_SET(sockfd, &rfds);
		for (int i = 0; i < MAX_CLIENTS; i++) {
			sd = client_sock[i];
			if (sd > 0) {
				FD_SET(sd, &rfds);
			}
			if (sd > max_sd) {
				max_sd = sd;
			}
		}

		select_ret = select(max_sd + 1, &rfds, NULL, NULL, NULL);
		if (select_ret == -1) {
			std::cerr << "Select error" << std::endl;
			std::cerr << std::strerror(errno) << std::endl;
			continue;
		}

		if (FD_ISSET(sockfd, &rfds)) {
			connected_sock = accept(sockfd, (struct sockaddr*)&address, (socklen_t*)&addrlen);
			if (connected_sock == -1) {
				std::cerr << "Accept error" << std::endl;
				std::cerr << std::strerror(errno) << std::endl;
				exit(EXIT_FAILURE);
			}

			for (int i = 0; i < MAX_CLIENTS; i++) {
				if (client_sock[i] == 0) {
					client_sock[i] = connected_sock;
					break;
				}
			}
		}

		for (int i = 0; i < MAX_CLIENTS; i++) {
			if (FD_ISSET(client_sock[i], &rfds)) {
				count = recv(client_sock[i], buffer, BUFFER_SIZE, 0);
				if (count == -1) {
					std::cerr << "Recv error" << std::endl;
					std::cerr << std::strerror(errno) << std::endl;
					exit(EXIT_FAILURE);
				}
				if (count < 3) {
					close(client_sock[i]);
					client_sock[i] = 0;
				} else {
					buffer[count] = '\0';
					std::cout << buffer;

					ss.str("");
					ss.clear();
					body.str("");
					body.clear();
					body << "<html><head><title>WEBSERV</title></head><body style=\"font-size: 40px\">";
					body << "Hello from server!";
					body << "</body></html>";
					ss << "HTTP/1.1 200 OK" << std::endl;
					ss << "Content-Type: text/html" << std::endl;
					ss << "Content-Length: " << body.str().length() << std::endl;
					ss << std::endl;
					ss << body.str();
					send(client_sock[i], ss.str().c_str(), ss.str().length(), 0);
				}
			}
		}
	}
}

int main() {
	int opt = 1;
	int sockfd;
	struct sockaddr_in address;
	int addrlen = sizeof(address);

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == -1) {
		std::cerr << "Cannot create socket" << std::endl;
		std::cerr << std::strerror(errno) << std::endl;
		exit(EXIT_FAILURE);
	}

	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) {
		std::cerr << "Setsockopt error" << std::endl;
		std::cerr << std::strerror(errno) << std::endl;
		exit(EXIT_FAILURE);
	}
	address.sin_family = AF_INET;
	/* address.sin_addr.s_addr = inet_addr("127.0.0.1"); */
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(PORT);

	if (bind(sockfd, (struct sockaddr*)&address, addrlen) == -1) {
		std::cerr << "Bind error" << std::endl;
		std::cerr << std::strerror(errno) << std::endl;
		exit(EXIT_FAILURE);
	}
	if (listen(sockfd, MAX_CLIENTS) == -1) {
		std::cerr << "Listen error" << std::endl;
		std::cerr << std::strerror(errno) << std::endl;
		exit(EXIT_FAILURE);
	}
	recieve(sockfd, address, addrlen);
	return 0;
}