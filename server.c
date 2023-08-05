#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>

#define PORT 8080

int main(int argc, char const *argv[])
{
    int server_fd, new_socket, valread, client_socket[2] = {0};
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};
    char* server_response = "Hello Client";

    // create server socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // set socket options to reuse address and port
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    // initialize server address structure
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // bind server socket to specified IP and port
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    // listen for incoming connections
    if (listen(server_fd, 2) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    printf("Server is listening on port %d\n", PORT);

    // loop to handle incoming client connections
    while (1) {
        // accept new client connection
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
            perror("accept");
            exit(EXIT_FAILURE);
        }

        // add client socket to array
        for (int i = 0; i < 2; i++) {
            if (client_socket[i] == 0) {
                client_socket[i] = new_socket;
                printf("Client with Port %d connected to server\n", ntohs(address.sin_port));
                break;
            }
        }

        // fork new process to handle client request
        if (fork() == 0) {
            close(server_fd); // close server socket in child process

            // read client message and print
            valread = read(new_socket, buffer, 1024);
            printf("Message from Client with Port %d: %s\n", ntohs(address.sin_port), buffer);

            // send response to client
            send(new_socket, server_response, strlen(server_response), 0);
            printf("Response sent to Client with Port %d\n", ntohs(address.sin_port));

            // close client socket
            close(new_socket);
            exit(0); // exit child process
        }
    }

    // close server socket
    close(server_fd);

    return 0;
}
