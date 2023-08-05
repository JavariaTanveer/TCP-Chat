#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 8080

int main() {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in server_addr = {
        .sin_family = AF_INET,
        .sin_port = htons(SERVER_PORT),
        .sin_addr.s_addr = inet_addr(SERVER_IP)
    };

    if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("connection failed");
        exit(EXIT_FAILURE);
    }

    char buffer[1024] = {0};
    printf("Enter message to send to server: ");
    fgets(buffer, 1024, stdin);
    send(sock, buffer, strlen(buffer), 0);

    char received_message[1024] = {0};
    ssize_t bytes_received = recv(sock, received_message, 1024, 0);
    if (bytes_received == -1) {
        perror("failed to receive message from server");
        exit(EXIT_FAILURE);
    } else if (bytes_received == 0) {
        printf("server disconnected\n");
    } else {
        printf("Received message from server: %s\n", received_message);
    }

    close(sock);
    return 0;
}
