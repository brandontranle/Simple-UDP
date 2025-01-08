#include "libsocket.h"
#include <arpa/inet.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

int main(int argc, char** argv) {
    if (argc < 3) {
        fprintf(stderr, "Usage: client <hostname> <port> \n");
        exit(1);
    }

    // Only supports localhost as a hostname, but that's all we'll test on
    const char* addr =
        strcmp(argv[1], "localhost") == 0 ? "127.0.0.1" : argv[1];
    int port = atoi(argv[2]);

    // TODO: Create socket
    int sockfd = s_socket(AF_INET, SOCK_DGRAM 0);
    if (sockfd < 0) {
        perror("socket");
        exit(1);
    }

    // TODO: Set stdin and socket nonblocking
    int flags = fcntl(STDIN_FILENO, F_GETFL, 0);
    flags |= O_NONBLOCK;
    fcntl(STDIN_FILENO, F_SETFL, flags);

    flags = fcntl(sockfd, F_GETFL, 0);
    flags |= O_NONBLOCK;
    fcntl(sockfd, F_SETFL, flags);

    // TODO: Construct server address
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = inet_addr(addr);
    
    // buffer for recciving data
    int BUF_SIZE = 1024;
    char buffer[BUF_SIZE];
    socklen_t serversize = sizeof(struct sockaddr_in);
    
    // Listen loop
    while (1) {    
        // TODO: Receive from socket
        int bytes_received = s_recvfrom(sockfd, buffer, BUF_SIZE, 0, (struct sockaddr*)&server_addr, &serversize);

        if (bytes_received <= 0) continue;
    
        // TODO: If data, write to stdout
        write(STDOUT_FILENO, buffer, bytes_received);

        // TODO: Read from stdin
        int bytes_read = read(STDIN_FILENO, buffer, BUF_SIZE);
        if (bytes_read < 0) continue;

        // TODO: If data, send to socket
        if (bytes_read > 0) {
            int bytes_sent = s_sendto(sockfd, buffer, bytes_read, 0, (struct sockaddr*)&server_addr, serversize);
            if (bytes_sent < 0) return errno;
        }
    }
    close(sockfd);
    return 0;
}
