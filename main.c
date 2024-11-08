#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <unistd.h> 
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

char *response_body(char *path, char *status){
    char *res_body = (char*) malloc(100 * (sizeof(char)));

    if (strcmp(path, "/home") == 0|| strcmp(path, "/") == 0) {
        strcpy(res_body, "<html><body><h1>Welcome to the Home Page</h1></body></html>");
    } else if(strcmp(path, "/about") == 0){
        strcpy(res_body, "<html><body><h1>Welcome to the About Page</h1></body></html>");
    } else {
        strcpy(res_body, "<html><body><h1>Page not found:(</h1></body></html>");
        strcpy(status, "404 NOT FOUND");
    };
    return res_body;
};

void *handle_client(void *arg){
    int client_fd = *((int*) arg); // type cast to int pointer and convert to int value by dereference
    
    // Read from client request
	    char buf[1024];
	    long buflen = sizeof(buf);
        int bytes_received =  recv(client_fd, buf, buflen, 0);
	    if(bytes_received <= 0) {
            perror("RECV\n");
	    }

        // Extract from request buffer
	    char method[10], path[50];
	    sscanf(buf, "%s %s", method, path);

	    // Get response body and status as per the route hit
        char status[40] = "200 OK";
	    char* res_body = response_body(path, status);
        int res_body_len = strlen(res_body);

        // Create http response
        char response[4098];
	    snprintf(response, sizeof response, 
            "HTTP/1.1 %s\r\n"
            "Content-Type: text/html\r\n"
            "Content-Length: %d\r\n"
            "\r\n"
            "%s",
            status, res_body_len, res_body
          );
        
        // Send response to the client 
        send(client_fd, response, strlen(response),  0);  
        printf("Sent response to client\n");

        // Clear up memory and close connection
        free(res_body);
        close(client_fd);
        
        return NULL;
}

int main() { 
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(8080);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    // Create a socket
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd < 0){
        perror("SOCKET FILE DESC");
        exit(EXIT_FAILURE);
    }

    // Bind the socket
    if(bind(sockfd, (struct sockaddr *)&server_addr, sizeof server_addr) < 0){
        perror("BIND");
        exit(EXIT_FAILURE);
    };
    printf("Socket bound\n");

    // Listen on the socket
    if(listen(sockfd, 3) < 0 ){
        perror("LISTEN");
        exit(EXIT_FAILURE);
    };
    printf("Listening on port 8080\n");
    
    while(1) {
        // Accept connections on socket
        int client_fd = accept(sockfd, 0, 0);

        // Spin up a thread to handle client request
        pthread_t tid;
        if(pthread_create(&tid, NULL, handle_client, (void*)&client_fd) != 0 ) {
            close(client_fd);
            perror("PTHREAD_CREATE");
            continue;
        }
        pthread_detach(tid);
	                               
    };
    close(sockfd);                                                               
    return 0;                                                                        
}       
