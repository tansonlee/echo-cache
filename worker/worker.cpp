#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h> // for close
#include <string.h>
#include <string>
#include <parser.h>
#include <iostream>
#include <sstream> 
#include <cache.h>

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

    sockaddr_in server_addr = build_server_info(port);
    int server_sockfd = build_server_fd(server_addr);
    printf("listen success.\n");

    char recv_buf[65536];
    memset(recv_buf, '\0', sizeof(recv_buf));

    Cache cache;

    while (true) {
        sockaddr_in client_addr;
        socklen_t length = sizeof(client_addr);
        // block on accept until positive fd or error
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

    printf("closed. \n");
    close(server_sockfd);
    return 0;
}