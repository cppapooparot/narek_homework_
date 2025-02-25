#include <iostream>
#include <unistd.h>
#include <string>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <errno.h>
#include <cstring>

int client_socket;

void* receive_messages(void*) {
    char buffer[1024];
    while (true) {
        int rs = recv(client_socket, buffer, sizeof(buffer) - 1, 0);
        if (rs <= 0) {
            break;
        }
        buffer[rs] = '\0';
        std::cout << buffer << std::endl;
    }
    return nullptr;
}

int main() {
    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = inet_addr("127.0.0.1");
    server_address.sin_port = htons(8888);

    if (connect(client_socket, (struct sockaddr*)&server_address, sizeof(server_address)) == -1) {
        perror("Connection failed");
        close(client_socket);
        exit(EXIT_FAILURE);
    }

    pthread_t recv_thread;
    pthread_create(&recv_thread, nullptr, receive_messages, nullptr);
    pthread_detach(recv_thread);

    char message[1024];

    while (true) {
        std::cin >> message;

        if (send(client_socket, message, strlen(message), 0) == -1) {
            perror("Send failed");
            break;
        }

        if (strcmp(message, "exit") == 0) {
            break;
        }
    }

    close(client_socket);
    return 0;
}
