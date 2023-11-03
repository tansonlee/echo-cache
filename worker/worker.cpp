#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h> // for close

#include <string>
#include <iostream>

#include <parser.h>
#include <cache.h>
#include <socket_server.h>

HandlerResponse handleRequest(char* buff, Cache& cache) {
    std::string command(buff);

    // Determine and parse the command.
    CommandType commandType = getCommandType(command);

    if (commandType == CommandType::get) {
        ParsedGetRequest getRequest = parseGet(command);
        if (getRequest.success == false) {
            return {StatusCode::parsingFailure, "Could not parse get request: '" + command + "'"};
        } else {
            return cache.get(getRequest.key);
        }
    }
    else if (commandType == CommandType::set) {
        ParsedSetRequest setRequest = parseSet(command);
        if (setRequest.success == false) {
            return {StatusCode::parsingFailure, "Could not parse set request: '" + command + "'"};
        } else {
            return cache.set(setRequest.key, setRequest.val);
        }
    }
    else if (commandType == CommandType::del) {
        ParsedDelRequest delRequest = parseDel(command);
        if (delRequest.success == false) {
            return {StatusCode::parsingFailure, "Could not parse del request: '" + command + "'"};
        } else {
            return cache.del(delRequest.key);
        }
    }
    else {
        return {StatusCode::invalidCommand, "Invalid command type: '" + command + "'"};
    }
}

int main(int argc, char *argv[]) {
    // Get port from command line arg.
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <port>" << std::endl;
        return 1;
    }
    int port = atoi(argv[1]);
    if (port == 0) {
        std::cerr << "Invalid port number" << std::endl;
        return 1;
    }

    int server_sockfd = buildSocketServer(port);
    std::cout << "Listening on port: " << port << std::endl;

    Cache cache;
    char recv_buf[65536];
    memset(recv_buf, '\0', sizeof(recv_buf));

    while (true) {
        sockaddr_in client_addr;
        socklen_t length = sizeof(client_addr);

        int conn = accept(server_sockfd, (struct sockaddr*)&client_addr,&length);
        if (conn < 0) {
            perror("connect");
            continue;
        }
        
        ssize_t bytes_received = recv(conn, recv_buf, sizeof(recv_buf), 0);
        if (bytes_received == -1) {
            perror("recv");
        } else if (bytes_received == 0) {
            // Connection closed by the client
        } else {
            HandlerResponse response = handleRequest(recv_buf, cache);
            std::string responseString = formatResponseString(response);

            int responseCode = send(conn, responseString.c_str(), responseString.size(), 0);
            if (responseCode == -1) {
                perror("send");
            }
            memset(recv_buf, '\0', sizeof(recv_buf));
        }
        close(conn);
    }

    std::cout << "Server shutdown" << std::endl;
    close(server_sockfd);
    return 0;
}