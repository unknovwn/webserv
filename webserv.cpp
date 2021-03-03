#include <sys/socket.h>
#include <sys/errno.h>
#include <string.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <iostream>

#define BUFFER_SIZE 1024
#define PORT 8080

int main() {
	int opt = 1;
	int sockfd;
	int connected_sock;
	struct sockaddr_in address;
	int addrlen = sizeof(address);
	char buffer[BUFFER_SIZE];

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == -1) {
		std::cerr << "Cannot create socket" << std::endl;
		exit(EXIT_FAILURE);
	}

	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) {
		std::cerr << "Setsockopt error" << std::endl;
		std::cerr << strerror(errno) << std::endl;
		exit(EXIT_FAILURE);
	}
	bzero(&address, sizeof(address));
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = inet_addr("127.0.0.1");
	address.sin_port = htons(PORT);

	if (bind(sockfd, (struct sockaddr*)&address, addrlen) == -1) {
		std::cerr << "Bind error" << std::endl;
		std::cerr << strerror(errno) << std::endl;
		exit(EXIT_FAILURE);
	}
	if (listen(sockfd, 3) == -1) {
		std::cerr << "Listen error" << std::endl;
		std::cerr << strerror(errno) << std::endl;
		exit(EXIT_FAILURE);
	}

	connected_sock = accept(sockfd, (struct sockaddr*)&address, (socklen_t*)&addrlen);
	if (connected_sock == -1) {
		std::cerr << "Accept error" << std::endl;
		std::cerr << strerror(errno) << std::endl;
		exit(EXIT_FAILURE);
	}
	if (recv(connected_sock, buffer, BUFFER_SIZE, 0) == -1) {
		std::cerr << "Recv error" << std::endl;
		std::cerr << strerror(errno) << std::endl;
		exit(EXIT_FAILURE);
	}
	std::cout << buffer << std::endl;
	const char* hello = "Hello from server\r\n";
	send(connected_sock, hello, strlen(hello), 0);
	std::cout << "Hello message sent" << std::endl;
	return 0;
}
