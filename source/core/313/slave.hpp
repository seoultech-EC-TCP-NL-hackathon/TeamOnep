//
// Created by dlwog on 25. 10. 30..
//

#ifndef MYPROJECT_SLAVE_HPP
#define MYPROJECT_SLAVE_HPP

#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>

int main() {
  int sock = socket(AF_INET, SOCK_STREAM, 0);
  sockaddr_in serv_addr{};
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(8080);
  inet_pton(AF_INET, "192.168.0.2", &serv_addr.sin_addr);

  connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
  send(sock, "Hello from client", 17, 0);

  char buffer[1024] = {0};
  read(sock, buffer, 1024);
  std::cout << "Server says: " << buffer << std::endl;
  close(sock);
}
#endif //MYPROJECT_SLAVE_HPP