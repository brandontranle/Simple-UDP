#include "libsocket.h"
#include <arpa/inet.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include "utils.h"

int main(int argc, char** argv) {
    if (argc < 3) {
        fprintf(stderr, "Usage: client <hostname> <port>\n");
        exit(1);
    }

    // Only supports localhost as a hostname, but that's all we'll test on
    const char* addr = strcmp(argv[1], "localhost") == 0 ? "127.0.0.1" : argv[1];
    int port = atoi(argv[2]);

    // Create socket
    int sockfd = s_socket(AF_INET, SOCK_DGRAM, 0);

    // Set stdin and socket nonblocking
    set_nonblocking(sockfd);
    set_nonblocking(STDIN_FILENO);
    
    // Construct server address
    char buffer[1024];

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = inet_addr(addr);
    socklen_t server_size = sizeof(server_addr);

    while (1) {
        // Receive from socket
        int bytes_received = s_recvfrom(sockfd, buffer, sizeof(buffer), 0, 
                                        (struct sockaddr*)&server_addr, &server_size);
        
        // If data, write to stdout
        if (bytes_received > 0) {
            write(STDOUT_FILENO, buffer, bytes_received);
        }

        // Read from stdin
        int bytes_read = read(STDIN_FILENO, buffer, sizeof(buffer));

        // If data, send to socket
        if (bytes_read > 0) { 
            s_sendto(sockfd, buffer, bytes_read, 0, (struct sockaddr*)&server_addr, server_size);
        }
    }

    close(sockfd);
    return 0;
}
