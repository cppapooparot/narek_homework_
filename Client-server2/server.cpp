#include <iostream>
#include <vector>
#include <unistd.h>
#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <cstring>
#include <algorithm>

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
std::vector<std::pair<int, std::string>> clients;
int client_count = 0;

void broadcast_message(const std::string& message, int sender_socket) {
    pthread_mutex_lock(&mutex);
    for (const auto& client : clients) {
        if (client.first != sender_socket) {
            send(client.first, message.c_str(), message.size(), 0);
        }
    }
    pthread_mutex_unlock(&mutex);
}

void* handle_client(void* arg) {
    int client_socket = *(int*)arg;
    delete (int*)arg;

    pthread_mutex_lock(&mutex);
    std::string client_name = "client" + std::to_string(++client_count);
    clients.emplace_back(client_socket, client_name);
    pthread_mutex_unlock(&mutex);

    std::string welcome_message = "Welcome, " + client_name + "!";
    send(client_socket, welcome_message.c_str(), welcome_message.size(), 0);

    char buffer[1024];

    while (true) {
        int rs = recv(client_socket, buffer, sizeof(buffer) - 1, 0);
        if (rs <= 0) {
            break;
        }

        buffer[rs] = '\0';
        std::string full_message = client_name + ": " + buffer;

        pthread_mutex_lock(&mutex);
        std::cout << full_message << std::endl;
        pthread_mutex_unlock(&mutex);

        broadcast_message(full_message, client_socket);

        if (strcmp(buffer, "exit") == 0) {
            break;
        }
    }

    pthread_mutex_lock(&mutex);
    clients.erase(std::remove_if(clients.begin(), clients.end(),
                 [client_socket](const std::pair<int, std::string>& client) {
                     return client.first == client_socket;
                 }),
                 clients.end());
    pthread_mutex_unlock(&mutex);

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
