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

std::mutex clientConnectionsMutex;
std::map<int, ConnectionUsage> clientConnections;

std::string handleRequest(char* buff, const std::string& worker_ip, int worker_port) {
    std::string command(buff);

    SocketClient client{worker_ip, worker_port};
    if (!client.connectionSucceeded) {
        return "";
    }
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

        if (bytes_received == -1) {
            perror("recv");
            std::cerr << "Exiting this listening thread" << std::endl;
            break;
        }
        if (bytes_received == 0) {
            std::cout << printPrefix << "connection closed by client" << std::endl;
            close(connection);
            std::lock_guard<std::mutex> lock(clientConnectionsMutex);
            clientConnections.erase(id);
            break;
        } 
        std::cout << printPrefix << "got " << recv_buf << std::endl;

        std::string command = recv_buf;
        if (command == "end") {
            std::cout << printPrefix << "client requests connection close" << std::endl;
            close(connection);
            std::lock_guard<std::mutex> lock(clientConnectionsMutex);
            clientConnections.erase(id);
            break;
        }

        ParsedKey parsedKey = parseKey(command);
        CommandType commandType = getCommandType(command);
        if (!parsedKey.success || commandType == CommandType::other) {
            break;
        }

        // Update connections lastUsed time.
        {
            std::lock_guard<std::mutex> lock(clientConnectionsMutex);
            clientConnections[id].lastUsed = getCurrentTime();
        }

        int workerIndex1 = hashKey(parsedKey.key, commandLineArguments.numWorkers);
        int workerIndex2 = (workerIndex1 + 1) % commandLineArguments.numWorkers;
        IpAndPort workerIpAndPort1 = commandLineArguments.workers[workerIndex1];
        IpAndPort workerIpAndPort2 = commandLineArguments.workers[workerIndex2];

        if (commandType == CommandType::set) {
            // Send to both workers.
            std::string response1 = handleRequest(recv_buf, workerIpAndPort1.ip, workerIpAndPort1.port);
            std::string response2 = handleRequest(recv_buf, workerIpAndPort2.ip, workerIpAndPort2.port);
            int responseCode = send(connection, response1.c_str(), response1.size(), 0);
            if (responseCode == -1) {
                perror("send");
            }
        }
        else if (commandType == CommandType::get) {
            // Send to first worker.
            std::string response1 = handleRequest(recv_buf, workerIpAndPort1.ip, workerIpAndPort1.port);
            if (!response1.empty()) {
                int responseCode = send(connection, response1.c_str(), response1.size(), 0);
                if (responseCode == -1) {
                    perror("send");
                }
            } else {
                // If it failed, send to second worker.
                std::string response2 = handleRequest(recv_buf, workerIpAndPort2.ip, workerIpAndPort2.port);
                int responseCode = send(connection, response2.c_str(), response2.size(), 0);
                if (responseCode == -1) {
                    perror("send");
                }
            }
        }
    }
    std::cout << printPrefix << "end connection" << std::endl;
}

void cleanUpThreads() {
    while (true) {
        {
            std::lock_guard<std::mutex> lock(clientConnectionsMutex);
            for (auto it = clientConnections.cbegin(); it != clientConnections.cend();) {
                int lastUsed = it->second.lastUsed;
                int currentTime = getCurrentTime();
                if (currentTime - lastUsed > 5) {
                    int connection = it->second.connection;
                    close(connection);
                    it = clientConnections.erase(it);
                } else {
                    ++it;
                }
            }
            std::cout << "Number of active threads: " << clientConnections.size() << std::endl;
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
        if (conn < 0) {
            perror("Orchestrator error connect");
            continue;
        }

        std::string client_ip = inet_ntoa(client_addr.sin_addr);
        int client_port = (int) ntohs(client_addr.sin_port);
        std::string printPrefix = client_ip + ":" + std::to_string(client_port) + " - ";

        int threadId = getNextId();
        clientHandlerThreads.push_back(std::thread(handleClient, commandLineArguments, conn, printPrefix, threadId));

        std::lock_guard<std::mutex> lock(clientConnectionsMutex);
        clientConnections[threadId] = { conn, getCurrentTime() };
    }

    std::cout << "Closing server" << std::endl;
    close(server_sockfd);
    return 0;
}
