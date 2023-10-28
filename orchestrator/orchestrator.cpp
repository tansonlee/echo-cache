#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h> // for close
#include <limits.h>

#include <string>
#include <iostream>
#include <thread>
#include <vector>
#include <chrono>
#include <map>
#include <mutex>

#include <parser.h>
#include <socket_client.h>
#include <socket_server.h>
#include "helpers.h"

// How often to check for zombie threads.
const int CLEAN_UP_THREAD_PERIOD_SEC = 60;
// How long with no activity until a thread is a zombie.
const int MAX_NO_ACTIVITY_LIFETIME_SEC = 60;

struct ConnectionUsage {
    int connection;
    int lastUsed;
};

std::map<int, ConnectionUsage> clientConnections;

std::string handleRequest(char* buff, const std::string& worker_ip, int worker_port) {
    std::string command(buff);

    SocketClient client{worker_ip, worker_port};
    client.sendMessage(command);
    std::string response = client.receiveResponse();
    return response;
}

void handleClient(CommandLineArguments commandLineArguments, int connection, const std::string& printPrefix, int id) {
    std::cout << printPrefix << "new client" << std::endl;
    char recv_buf[65536];
    
    while (true) {
        memset(recv_buf, '\0', sizeof(recv_buf));
        ssize_t bytes_received = recv(connection, recv_buf, sizeof(recv_buf), 0);

        std::cout << printPrefix << "got " << recv_buf << std::endl;

        if (bytes_received == -1) {
            perror("recv");
            std::cerr << "Exiting this listening thread" << std::endl;
            break;
        }
        if (bytes_received == 0) {
            std::cout << printPrefix << "connection closed by client" << std::endl;
            close(connection);
            clientConnections.erase(id);
            break;
        } 

        std::string command = recv_buf;
        if (command == "end") {
            std::cout << printPrefix << "client requests connection close" << std::endl;
            std::string response = "closing";
            int responseCode = send(connection, response.c_str(), response.size(), 0);
            close(connection);
            clientConnections.erase(id);
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
    // close(connection);
}

void cleanUpThreads() {
    while (true) {
        // Perform cleanup
        for (auto it = clientConnections.cbegin(); it != clientConnections.cend();) {
            std::cerr << "Looking at thread" << std::endl;
            int lastUsed = it->second.lastUsed;
            int currentTime = getCurrentTime();
            std::cout << "time diff " << currentTime - lastUsed << std::endl;
            if (currentTime - lastUsed > 5) {
                std::cerr << "killing this thread" << std::endl;
                int connection = it->second.connection;
                close(connection);
                it = clientConnections.erase(it);
            } else {
                ++it;
            }
        }

        std::this_thread::sleep_for(std::chrono::seconds(5));
    }
}

int main(int argc, char *argv[]) {
    CommandLineArguments commandLineArguments = parseCommandLineArguments(argc, argv);
    if (!commandLineArguments.success) {
        std::cerr << "Usage: " << argv[0] << " <orchestrator port> <worker ip 1> <worker port 1> ... <worker ip n> <worker port n>" << std::endl;
        return 1;
    }

    int server_sockfd = buildSocketServer(commandLineArguments.port);
    std::cout << "Listening on port: " << commandLineArguments.port << std::endl;

    std::vector<std::thread> clientHandlerThreads;
    std::thread custodianThread = std::thread(cleanUpThreads);

    while (true) {
        sockaddr_in client_addr;
        socklen_t length = sizeof(client_addr);
        int conn = accept(server_sockfd, (struct sockaddr*)&client_addr,&length);
        // block on accept until positive fd or error
        if(conn<0) {
            perror("Orchestrator error connect");
            continue;
        }

        std::string client_ip = inet_ntoa(client_addr.sin_addr); int client_port = (int) ntohs(client_addr.sin_port);
        std::string printPrefix = client_ip + ":" + std::to_string(client_port) + " - ";

        int threadId = getNextId();
        clientHandlerThreads.push_back(std::thread(handleClient, commandLineArguments, conn, printPrefix, threadId));

        clientConnections[threadId] = { conn, getCurrentTime() };
    }

    std::cout << "Closing server" << std::endl;
    close(server_sockfd);
    return 0;
}
