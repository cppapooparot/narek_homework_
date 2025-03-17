#include <iostream>
#include <cstring>
#include <unistd.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <sys/stat.h>

#define SERVER "httpforever.com"
#define PORT 80
#define FILENAME "httpforever.html"

std::string get_ip(const char* host)
{
    struct hostent* host_info;
    struct in_addr** addr_list;

    if ((host_info = gethostbyname(host)) == nullptr)
    {
        std::cerr << "Failed while getting host" << std::endl;
        exit(1);
    }
    
    addr_list = (struct in_addr**)host_info->h_addr_list;
    return inet_ntoa(*addr_list[0]);
}

int main()
{
    int socket_fd;
    struct sockaddr_in server_addr;
    char buffer[4096];
    const char* host = SERVER;
    
    std::string server_ip = get_ip(host);
    
    if ((socket_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        std::cerr << "Failed while creating socket" << std::endl;
        exit(1);    
    }
    
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr(server_ip.c_str());

    if (connect(socket_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1)
    {
        std::cerr << "Failed to connect" << std::endl;
        exit(1);
    }
    
    const char *request = "GET / HTTP/1.1\r\nHost: httpforever.com\r\nConnection: close\r\n\r\n";
    
    if (send(socket_fd, request, strlen(request), 0) == -1)
    {
        std::cerr << "Failed while sending request to server" << std::endl;
        exit(1);        
    }

    int fd = open(FILENAME, O_WRONLY | O_CREAT | O_TRUNC, 0666);
    if (fd == -1)
    {
        std::cerr << "Failed to open the file" << std::endl;
        exit(1);
    }

    int bytes_received;
    while ((bytes_received = recv(socket_fd, buffer, sizeof(buffer), 0)) > 0)
    {
        if (write(fd, buffer, bytes_received) == -1)
        {
            std::cerr << "Failed to write data to file" << std::endl;
            exit(1);
        }
    }

    if (bytes_received == -1)
    {
        std::cerr << "Failed to receive data" << std::endl;
        exit(1);
    }

    std::cout << "HTML content written to " << FILENAME << std::endl;

    close(fd);
    close(socket_fd);

    return 0;
}
