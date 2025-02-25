#include <iostream>
#include <unistd.h>
#include <string>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <errno.h>

int main() {
    int client_socket = socket(AF_INET, SOCK_STREAM, 0);
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

    std::cout << "Enter a message ('exit' to quit):" << std::endl;
    std::string message;
    
    while (true) {
	    std::cin >> message;

        if (send(client_socket, message.c_str(), message.size(), 0) == -1) {
            perror("Send failed");
            break;
        }

        if (message == "exit") {
            break;
        }
    }

    close(client_socket);
    return 0;
}
