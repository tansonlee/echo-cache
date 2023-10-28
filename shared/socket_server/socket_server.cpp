#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

sockaddr_in buildServerAddress(int port) {
    sockaddr_in server_addr;                // server info struct
    server_addr.sin_family=AF_INET;         // TCP/IP
    server_addr.sin_addr.s_addr=INADDR_ANY; // server addr--permit all connection
    server_addr.sin_port=htons(port);       // server port

    return server_addr;
}

int buildServerFileDescriptor(sockaddr_in server_addr) {
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

int buildSocketServer(int port) {
    sockaddr_in serverAddr = buildServerAddress(port);
    int serverSockfd = buildServerFileDescriptor(serverAddr);

    return serverSockfd ;
}