#include <iostream>
#include <unistd.h>
#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <cstring>

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void* handle_client(void* arg) {
    int client_socket = *(int*)arg;
    delete (int*)arg;

    char buffer[1024];

    while (true) {
        int rs = recv(client_socket, buffer, sizeof(buffer) - 1, 0);
        if (rs <= 0) {
            break;
        }
        buffer[rs] = '\0';

        pthread_mutex_lock(&mutex);
        std::cout << "Received: " << buffer << std::endl;
        pthread_mutex_unlock(&mutex);
    }

    close(client_socket);
    return nullptr;
}

int main() {
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = htonl(INADDR_ANY);
    server_address.sin_port = htons(8888);

    if (bind(server_socket, (struct sockaddr*)&server_address, sizeof(server_address)) < 0) {
        perror("Bind failed");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    if (listen(server_socket, 10) < 0) {
        perror("Listen failed");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    std::cout << "Server is running..." << std::endl;

    while (true) {
        struct sockaddr_in client_address;
        socklen_t client_addr_len = sizeof(client_address);
        int* client_socket = new int(accept(server_socket, (struct sockaddr*)&client_address, &client_addr_len));

        if (*client_socket == -1) {
            perror("Accept failed");
            delete client_socket;
            continue;
        }

        pthread_t thread;
        pthread_create(&thread, nullptr, handle_client, client_socket);
        pthread_detach(thread);
    }

    close(server_socket);
    pthread_mutex_destroy(&mutex);
    return 0;
}
