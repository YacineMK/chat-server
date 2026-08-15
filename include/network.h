#ifndef NETWORK_H
#define NETWORK_H

#include <netinet/in.h>

int create_socket(void);
int setup_server_socket(int server_socket,struct sockaddr_in *server_address,int port);
int start_server(int server_socket);
int accept_client(int server_socket);

#endif