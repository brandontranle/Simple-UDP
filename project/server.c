#include "libsocket.h"
#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include "utils.h"

int main(int argc, char** argv) {
    if (argc < 2) {
        fprintf(stderr, "Usage: server <port>\n");
        exit(1);
    }

    int port = atoi(argv[1]);

    // Create socket
    int socketfd = s_socket(AF_INET, SOCK_DGRAM, 0);

    // Set socket nonblocking
    set_nonblocking(socketfd);
    set_nonblocking(STDIN_FILENO);

    // Construct server address
    struct sockaddr_in servaddr;
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(port);

    // Bind address to socket
    int did_bind = s_bind(socketfd, (struct sockaddr*)&servaddr, sizeof(servaddr));
    if (did_bind < 0) return errno;
   
    // Create sockaddr_in and socklen_t buffers to store client address
    char buffer[1024];
    int client_connected = 0; 
    struct sockaddr_in clientaddr = {0};
    socklen_t clientaddr_len = sizeof(clientaddr);

    while (1) {
        // Receive from socket
        int bytes_received = s_recvfrom(socketfd, buffer, sizeof(buffer), 0, 
                                        (struct sockaddr*)&clientaddr, &clientaddr_len);

        // If data is received, mark client as connected and write to stdout
        if (bytes_received > 0) {
            client_connected = 1;
            write(STDOUT_FILENO, buffer, bytes_received);
        }

        // If no client is connected, continue the loop
        if (!client_connected) { 
            continue;
        }

        // Read from stdin
        int bytes_read = read(STDIN_FILENO, buffer, sizeof(buffer));

        // If data, send to socket
        if (bytes_read > 0) { 
            while (bytes_read > 0) { //while no error 
                int bytes_sent = s_sendto(socketfd, buffer, bytes_read, 0, (struct sockaddr*)&clientaddr, clientaddr_len); // Send the data from standard input to the client
                if (bytes_sent < 0) return errno;
                bytes_read -= bytes_sent; //remaining bytes sent in iterations
            }
        }
    }

    close(socketfd);
    return 0;
}
