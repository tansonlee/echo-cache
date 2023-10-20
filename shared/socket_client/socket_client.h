#ifndef SOCKET_CLIENT_H
#define SOCKET_CLIENT_H

#include <string>
#include <exception>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h> // for close

class SocketClient {
  public:
    SocketClient(const std::string& ip, int port);
    ~SocketClient();

    std::string sendMessage(const std::string& message);
  private:
    sockaddr_in server_addr;
    int sock_client;

};

#endif

