//
// Created by dlwog on 25. 10. 30..
//

#ifndef MYPROJECT_SERVER_HPP
#define MYPROJECT_SERVER_HPP



#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <iostream>

int main() {
  int server_fd = socket(AF_INET, SOCK_STREAM, 0);
  sockaddr_in address{};
  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port = htons(8080);

  bind(server_fd, (struct sockaddr*)&address, sizeof(address));
  listen(server_fd, 3);

  int addrlen = sizeof(address);
  int client_socket = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen);

  char buffer[1024] = {0};
  read(client_socket, buffer, 1024);
  std::cout << "Received: " << buffer << std::endl;

  send(client_socket, "Hello from server", 17, 0);
  close(client_socket);
  close(server_fd);
}

#endif //MYPROJECT_SERVER_HPP