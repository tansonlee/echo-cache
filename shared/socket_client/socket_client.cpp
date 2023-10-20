#include "socket_client.h"

#include <iostream>
#include <string>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h> // for close

SocketClient::SocketClient(const std::string& ip, int port) {
    memset(&this->server_addr, 0, sizeof(this->server_addr));
    this->server_addr.sin_family = AF_INET;
    this->server_addr.sin_addr.s_addr = inet_addr(ip.c_str());
    this->server_addr.sin_port = htons(port);  // server default port

    this->sock_client = socket(AF_INET,SOCK_STREAM, 0);

    if (this->sock_client < 0) {
        std::cerr << "Something went wrong" << std::endl;
    }
}

SocketClient::~SocketClient() {
    close(this->sock_client);
}

std::string SocketClient::sendMessage(const std::string& message) {
    char send_buf[65536];
    memset(send_buf, '\0', sizeof(send_buf));
    strcpy(send_buf, message.c_str());

    //connect server, return 0 with success, return -1 with error
    int connection = connect(this->sock_client, (struct sockaddr *)&this->server_addr, sizeof(this->server_addr));
    if (connection < 0) {
        perror("connect");
        return "";
    }

    // char server_ip[INET_ADDRSTRLEN]="";
    // inet_ntop(AF_INET, &this->server_addr.sin_addr, server_ip, INET_ADDRSTRLEN);
    // printf("connected server(%s:%d). \n", server_ip, ntohs(this->server_addr.sin_port));

    //send a message to server
    send(this->sock_client, send_buf, strlen(send_buf), 0);

    char recv_buf[65536]; 
    ssize_t bytes_received = recv(this->sock_client, recv_buf, sizeof(recv_buf), 0);
    if (bytes_received < 0) {
        perror("recv");
    } else if (bytes_received == 0) {
        std::cout << "Server closed the connection." << std::endl;
    } else {
        // Null-terminate the received data to treat it as a string
        recv_buf[bytes_received] = '\0';
    } 
    return recv_buf;
}


