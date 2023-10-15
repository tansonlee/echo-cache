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
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <server address> <server port>" << std::endl;
        return 1;
    }
    struct sockaddr_in server_addr;     // set server addr and port
    std::cerr << 1 << std::endl;
    memset(&server_addr, 0, sizeof(server_addr));
    std::cerr << 2 << std::endl;
    server_addr.sin_family = AF_INET;
    std::cerr << 3 << std::endl;
    server_addr.sin_addr.s_addr = inet_addr(argv[1]);
    std::cerr << 4 << std::endl;
    server_addr.sin_port = htons(atoi(argv[2]));  // server default port
    std::cerr << 5 << std::endl;

    int sock_client;
    std::cerr << 6 << std::endl;
    char send_buf[65536];
    std::cerr << 7 << std::endl;
    memset(send_buf, '\0', sizeof(send_buf));

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

    while (true) {
        std::string send_content;
        std::cin >> send_content;
        strcpy(send_buf, send_content.c_str());

        //send a message to server
        send(sock_client, send_buf, strlen(send_buf), 0);

        char recv_buf[65536]; 
        // receive response
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
        memset(send_buf, '\0', sizeof(send_buf));
    }

    close(sock_client);

    return 0;
}
