#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "network.h"

int create_socket(void) {
    int socket_fd;
    socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd < 0)
    {
        perror("Socket creation failed");
        return -1; 
    }
    return socket_fd;
}

int setup_server_socket(int server_socket,struct sockaddr_in *server_address,int port){
    server_address->sin_family = AF_INET;
    server_address->sin_port = htons(port);
    server_address->sin_addr.s_addr = INADDR_ANY;

    int a = bind(server_socket, (struct sockaddr *)server_address, sizeof(*server_address));
    if (a < 0)
    {
        perror("Bind failed");
        return -1;
    }
}

int start_server(int server_socket) {
    if (listen(server_socket, 5) == -1)
    {
        perror("listen");
        return -1;
    }
    return 0;
}

int accept_client(int server_socket) {
    int client_socket;
    client_socket = accept(
        server_socket,
        NULL,
        NULL
    );

    if (client_socket == -1)
    {
        perror("accept");
        return -1;
    }
    return client_socket;
}