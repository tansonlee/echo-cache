#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h> // for close
#include <string.h>
#include <string>
#include <iostream>
#include "../shared/parser.h"

sockaddr_in build_server_info(int port) {
    sockaddr_in server_addr;     // server info struct
    server_addr.sin_family=AF_INET;     // TCP/IP
    server_addr.sin_addr.s_addr=INADDR_ANY;     // server addr--permit all connection
    server_addr.sin_port=htons(port);       // server port

    return server_addr;
}

int build_server_fd(sockaddr_in server_addr) {
    int server_sockfd;      // server socket fd 
    /* create socket fd with IPv4 and TCP protocal*/
    if((server_sockfd=socket(PF_INET,SOCK_STREAM,0))<0) {  
        perror("socket error");
        return 1;
    }

    /* bind socket with server addr */
    if(bind(server_sockfd,(struct sockaddr *)&server_addr,sizeof(struct sockaddr))<0) {
        perror("bind error");
        return 1;
    }


    /* listen connection request with a queue length of 20 */
    if(listen(server_sockfd,20)<0) {
        perror("listen error");
        return 1;
    }

    return server_sockfd;
}

std::string handleRequest(char* buff, const char* worker_ip, int worker_port) {
    std::string command(buff);

    struct sockaddr_in server_addr;     // set server addr and port
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(worker_ip);
    server_addr.sin_port = htons(worker_port);  // server default port

    int sock_client;
    char send_buf[65536];
    memset(send_buf, '\0', sizeof(send_buf));
    strcpy(send_buf, command.c_str());

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
    return recv_buf;
}

struct IpAndPort {
    std::string ip;
    int port;
};

struct CommandLineArguments {
    bool success;
    int port;
    int numWorkers;
    IpAndPort* workers;
};

CommandLineArguments parseCommandLineArguments(int argc, char *argv[]) {
    // Ensure there are an even number of args.
    if (argc % 2 != 0) {
        return {false, 0, 0, {}};
    }

    int port = atoi(argv[1]);
    if (port == 0) {
        return {false, 0, 0, {}};
    }

    int numWorkers = (argc - 2) / 2;
    IpAndPort* workers = new IpAndPort[numWorkers];

    for (int i = 2; i < argc; i += 2) {
        std::string ip = argv[i];
        int port = atoi(argv[i + 1]);
        if (port == 0) {
            return {false, 0, 0, {}};
        }

        workers[(i - 2) / 2] = {ip, port};
    }

    return {true, port, numWorkers, workers};
}

// Gives an int from 0 to (numWorkers - 1)
int hashKey(std::string key, int numWorkers) {
    int result = 0;
    int len = key.length();
    for (int i = 0; i < len; ++i) {
        result += (int)key.at(i);
        result = result % numWorkers;
    }

    return result;
}

int main(int argc, char *argv[]) {
    CommandLineArguments commandLineArguments = parseCommandLineArguments(argc, argv);
    if (!commandLineArguments.success) {
        std::cerr << "Usage: " << argv[0] << " <orchestrator port> <worker ip 1> <worker port 1> ... <worker ip n> <worker port n>" << std::endl;
        return 1;
    }

    std::cerr << 1 << std::endl;
    sockaddr_in server_addr = build_server_info(commandLineArguments.port);
    std::cerr << 2 << std::endl;
    int server_sockfd = build_server_fd(server_addr);
    std::cerr << 3 << std::endl;
    printf("listen success.\n");

    char recv_buf[65536];
    memset(recv_buf, '\0', sizeof(recv_buf));

    while (true) {
        sockaddr_in client_addr;
        socklen_t length = sizeof(client_addr);
        // block on accept until positive fd or error
        std::cerr << 4 << std::endl;
        int conn = accept(server_sockfd, (struct sockaddr*)&client_addr,&length);
        if(conn<0) {
            perror("connect");
            continue;
        }

        printf("new client accepted.\n");

        char client_ip[INET_ADDRSTRLEN] = "";
        inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, INET_ADDRSTRLEN);

        
        ssize_t bytes_received = recv(conn, recv_buf, sizeof(recv_buf), 0);
        if (bytes_received == -1) {
            perror("recv");
        } else if (bytes_received == 0) {
            // Connection closed by the client
        } else {
            printf("recv %s from client(%s:%d).\n", recv_buf, client_ip, ntohs(client_addr.sin_port));

            std::string command = recv_buf;

            ParsedKey parsedKey = parseKey(command);
            if (parsedKey.success) {
                int hash = hashKey(parsedKey.key, commandLineArguments.numWorkers);
                std::cout << "SENDING TO WORKER " << hash << std::endl;

                IpAndPort workerIpAndPort = commandLineArguments.workers[hash];
                
                std::string response = handleRequest(recv_buf, workerIpAndPort.ip.c_str(), workerIpAndPort.port);

                int responseCode = send(conn, response.c_str(), response.size(), 0);
                if (responseCode == -1) {
                    perror("send");
                }
            }
            memset(recv_buf, '\0', sizeof(recv_buf));
        }
        close(conn);
    }

    printf("closed. \n");
    close(server_sockfd);
    return 0;
}
