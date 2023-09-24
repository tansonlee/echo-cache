#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <iostream>
#include <string>

int main(int argc, char **argv) {
    struct sockaddr_in server_addr;     // set server addr and port
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(argv[1]);
    server_addr.sin_port = htons(9000);  // server default port

    int sock_client;
    char send_buf[65536];
    memset(send_buf, '\0', sizeof(send_buf));
    std::string send_content;
    std::cin >> send_content;
    strcpy(send_buf, send_content.c_str());

    if ((sock_client = socket(AF_INET,SOCK_STREAM, 0)) < 0) {
        return 0;
    }

    //connect server, return 0 with success, return -1 with error
    if (connect(sock_client, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("connect");
        return 0;
    }

    char server_ip[INET_ADDRSTRLEN]="";
    inet_ntop(AF_INET, &server_addr.sin_addr, server_ip, INET_ADDRSTRLEN);
    printf("connected server(%s:%d). \n", server_ip, ntohs(server_addr.sin_port));

    //send a message to server
    send(sock_client, send_buf, strlen(send_buf), 0);

    char recv_buf[65536]; 
    ssize_t bytes_received = recv(sock_client, recv_buf, sizeof(recv_buf), 0);
    if (bytes_received < 0) {
        perror("recv");
    } else if (bytes_received == 0) {
        std::cout << "Server closed the connection." << std::endl;
    } else {
        // Null-terminate the received data to treat it as a string
        recv_buf[bytes_received] = '\0';
        std::cout << "Received from server: " << recv_buf << std::endl;
    } 

    close(sock_client);

    return 0;
}
