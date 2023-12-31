#include "socket_client.h"

#include <cstring>
#include <iostream>
#include <string>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

SocketClient::SocketClient(const std::string &ip, int port) {
  this->connectionSucceeded = true;
  memset(&this->server_addr, 0, sizeof(this->server_addr));
  this->server_addr.sin_family = AF_INET;
  this->server_addr.sin_addr.s_addr = inet_addr(ip.c_str());
  this->server_addr.sin_port = htons(port); // server default port

  this->sock_client = socket(AF_INET, SOCK_STREAM, 0);

  if (this->sock_client < 0) {
    this->connectionSucceeded = false;
    std::cerr << "Something went wrong" << std::endl;
  }

  int connection =
      connect(this->sock_client, (struct sockaddr *)&this->server_addr,
              sizeof(this->server_addr));
  if (connection < 0) {
    perror("connect");
    this->connectionSucceeded = false;
  }
}

SocketClient::~SocketClient() { close(this->sock_client); }

void SocketClient::sendMessage(const std::string &message) {
  if (!this->connectionSucceeded) {
    std::cerr << "Tried to send a message on a failed connection" << std::endl;
    return;
  }
  send(this->sock_client, message.c_str(), strlen(message.c_str()), 0);
}

std::string SocketClient::receiveResponse() {
  if (!this->connectionSucceeded) {
    std::cerr << "Tried to receive response on a failed connection"
              << std::endl;
    return "";
  }
  char recv_buf[65536];
  ssize_t bytes_received =
      recv(this->sock_client, recv_buf, sizeof(recv_buf), 0);
  if (bytes_received < 0) {
    perror("recv");
    return "";
  } else if (bytes_received == 0) {
    // Server closed the connection
    return "";
  } else {
    // Null-terminate the received data to treat it as a string
    recv_buf[bytes_received] = '\0';
  }
  return recv_buf;
}
