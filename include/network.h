#ifndef NETWORK_H
#define NETWORK_H

#include <netinet/in.h>

int create_socket(void);
int setup_server_socket(int server_socket, struct sockaddr_in *server_address, int port);
int start_server(int server_socket);
int accept_client(int server_socket);

int connect_to_server(int client_socket, const char *host, int port);

int send_message(int socket_fd, const char *message);
int receive_message(int socket_fd, char **message);

void *handle_client(void *arg);

#endif
