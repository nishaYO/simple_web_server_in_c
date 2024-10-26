#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h> 

int main() {
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(8080);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    bind(sockfd, (struct sockaddr *)&server_addr, sizeof server_addr); 
    listen(sockfd, 3);
    int client_fd = accept(sockfd, 0, 0);
    
    const char *http_response = 
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/plain\r\n"
        "Content-Length: 13\r\n"
        "\r\n"
        "Hello, world!";

    send(client_fd, http_response, strlen(http_response), 0);
    printf("Sent response to client\n");

    close(client_fd);
    close(sockfd);
    return 0;
}

