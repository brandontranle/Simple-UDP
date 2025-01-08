#include "libsocket.h"
#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <unistd.h>

int main(int argc, char** argv) {
    if (argc < 2) {
        fprintf(stderr, "Usage: server <port>\n");
        exit(1);
    }

    int port = atoi(argv[1]);

    // TODO: Create socket
    int socketfd = s_socket(AF_INET, SOCK_DGRAM, 0);
    
    if (socketfd < 0) {
        perror("socket");
        exit(1);
    }

    // TODO: Set stdin and socket nonblocking
    int flags = fcntl(STDIN_FILENO, F_GETFL, 0);
    flags |= O_NONBLOCK;
    fcntl(STDIN_FILENO, F_SETFL, flags);

    flags = fcntl(socketfd, F_GETFL, 0);
    flags |= O_NONBLOCK;
    fcntl(socketfd, F_SETFL, flags);

    // TODO: Construct server address
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET; // IPv4
    server_addr.sin_addr.s_addr = INADDR_ANY; // Accept all connections

    int server_port = htons(port);
    server_addr.sin_port = server_port;

    // TODO: Bind address to socket
    int did_bind = s_bind(socketfd, (struct sockaddr*)&server_addr, sizeof(server_addr));
    if (did_bind < 0) return errno;

    // TODO: Create sockaddr_in and socklen_t buffers to store client address
    int BUF_SIZE = 1024;
    char buffer[BUF_SIZE];
    int client_connected = 0;
    struct sockaddr_in client_addr = {0};
    socklen_t client_addr_len = sizeof(client_addr);

    // Listen loop
    while (1) {
        // TODO: Receive from socket
        int bytes_received = s_recvfrom(socketfd, buffer, BUF_SIZE, 0, (struct sockaddr*)&client_addr, &client_addr_len);
        // TODO: If no data and client not connected, continue
        if (bytes_received < 0 && !client_connected) continue;

        // TODO: If data, client is connected and write to stdout
        if (bytes_received > 0) {
            client_connected = 1;
            write(STDOUT_FILENO, buffer, bytes_received);
        }   
        // TODO: Read from stdin
        int bytes_read = read(STDIN_FILENO, buffer, BUF_SIZE);

        // TODO: If data, send to socket
        if (bytes_read > 0) {
            s_sendto(socketfd, buffer, bytes_read, 0, (struct sockaddr*)&client_addr, client_addr_len);
        }
    }
    close(socketfd);
    return 0;
}
