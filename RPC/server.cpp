#include <iostream>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024

double calculate(const char *operation, double a, double b) {
  if (strcmp(operation, "ADD") == 0) return a + b;
  if (strcmp(operation, "SUB") == 0) return a - b;
  if (strcmp(operation, "MUL") == 0) return a * b;
  if (strcmp(operation, "DIV") == 0) return (b != 0) ? a / b : 0;
  return 0;
}

int main() {
  int server_fd, client_fd;
  struct sockaddr_in server_addr, client_addr;
  socklen_t addr_len = sizeof(client_addr);
  char buffer[BUFFER_SIZE];

  server_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (server_fd == -1) {
    perror("socket failed");
    exit(EXIT_FAILURE);
  }

  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = INADDR_ANY;
  server_addr.sin_port = htons(PORT);

  if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
    perror("bind failed");
    close(server_fd);
    exit(EXIT_FAILURE);
  }

  if (listen(server_fd, 5) < 0) {
    perror("listen failed");
    close(server_fd);
    exit(EXIT_FAILURE);
  }

  std::cout << "Server listening on port " << PORT << std::endl;

  while (true) {
    client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &addr_len);
    if (client_fd < 0) {
      perror("accept failed");
      continue;
    }

    memset(buffer, 0, BUFFER_SIZE);
    read(client_fd, buffer, BUFFER_SIZE);

    char *token = strtok(buffer, " ");
    if (!token) {
      write(client_fd, "ERROR", 5);
      close(client_fd);
      continue;
    }

    char operation[BUFFER_SIZE];
    strcpy(operation, token);

    token = strtok(NULL, " ");
    if (!token) {
      write(client_fd, "ERROR", 5);
      close(client_fd);
      continue;
    }
    double a = atof(token);

    token = strtok(NULL, " ");
    if (!token) {
      write(client_fd, "ERROR", 5);
      close(client_fd);
      continue;
    }
    double b = atof(token);

    double result = calculate(operation, a, b);
    snprintf(buffer, BUFFER_SIZE, "%.2f", result);

    write(client_fd, buffer, strlen(buffer));
    close(client_fd);
  }

  close(server_fd);
  return 0;
}
