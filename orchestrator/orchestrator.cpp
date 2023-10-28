#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h> // for close
#include <string.h>
#include <string>
#include <iostream>
#include <parser.h>
#include <socket_client.h>

#include <thread>
#include <vector>

sockaddr_in build_server_info(int port) {
    sockaddr_in server_addr;     // server info struct
    server_addr.sin_family=AF_INET;     // TCP/IP
    server_addr.sin_addr.s_addr=INADDR_ANY;     // server addr--permit all connection
    server_addr.sin_port=htons(port);       // server port

    return server_addr;
}

int build_server_fd(sockaddr_in server_addr) {
    int server_sockfd;      // server socket fd 
    /* create socket fd with IPv4 and TCP protocol*/
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

std::string handleRequest(char* buff, const std::string& worker_ip, int worker_port) {
    std::string command(buff);

    SocketClient client{worker_ip, worker_port};
    std::string response = client.sendMessage(command);

    return response;
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

void handleClient(CommandLineArguments commandLineArguments, int connection, const std::string& printPrefix) {
    std::cout << printPrefix << "new client" << std::endl;
    char recv_buf[65536];
    
    while (true) {
        memset(recv_buf, '\0', sizeof(recv_buf));
        ssize_t bytes_received = recv(connection, recv_buf, sizeof(recv_buf), 0);

        std::cout << printPrefix << "got " << recv_buf << std::endl;

        if (bytes_received == -1) {
            perror("recv");
            break;
        }
        if (bytes_received == 0) {
            std::cout << printPrefix << "connection closed by client" << std::endl;
            break;
        } 

        std::string command = recv_buf;
        if (command == "end") {
            std::cout << printPrefix << "client requests connection close" << std::endl;
            std::string response = "closing";
            int responseCode = send(connection, response.c_str(), response.size(), 0);
            break;
        }

        ParsedKey parsedKey = parseKey(command);
        if (parsedKey.success) {
            int hash = hashKey(parsedKey.key, commandLineArguments.numWorkers);
            std::cout << printPrefix << "routing " << hash << std::endl;

            IpAndPort workerIpAndPort = commandLineArguments.workers[hash];
            
            std::string response = handleRequest(recv_buf, workerIpAndPort.ip, workerIpAndPort.port);

            std::cout << printPrefix << "respond " << response << std::endl;
            int responseCode = send(connection, response.c_str(), response.size(), 0);
            if (responseCode == -1) {
                perror("send");
            }
        }
    }
    std::cout << printPrefix << "end connection" << std::endl;
    close(connection);
}

int main(int argc, char *argv[]) {
    CommandLineArguments commandLineArguments = parseCommandLineArguments(argc, argv);
    if (!commandLineArguments.success) {
        std::cerr << "Usage: " << argv[0] << " <orchestrator port> <worker ip 1> <worker port 1> ... <worker ip n> <worker port n>" << std::endl;
        return 1;
    }

    sockaddr_in server_addr = build_server_info(commandLineArguments.port);
    int server_sockfd = build_server_fd(server_addr);
    std::cout << "Listening on port: " << commandLineArguments.port << std::endl;

    std::vector<std::thread> clientHandlerThreads;

    while (true) {
        sockaddr_in client_addr;
        socklen_t length = sizeof(client_addr);
        int conn = accept(server_sockfd, (struct sockaddr*)&client_addr,&length);
        // block on accept until positive fd or error
        if(conn<0) {
            perror("Orchestrator error connect");
            continue;
        }

        std::string client_ip = inet_ntoa(client_addr.sin_addr);
        int client_port = (int) ntohs(client_addr.sin_port);
        std::string printPrefix = client_ip + ":" + std::to_string(client_port) + " - ";

        clientHandlerThreads.push_back(std::thread(handleClient, commandLineArguments, conn, printPrefix));
        // handleClient(commandLineArguments, conn, printPrefix);
    }

    std::cout << "Closing server" << std::endl;
    close(server_sockfd);
    return 0;
}
