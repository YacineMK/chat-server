#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <netinet/in.h>

#include "network.h"

#define PORT 8080

int main(void)
{
    int server_socket;
    struct sockaddr_in server_address;

    server_socket = create_socket();
    if (server_socket == -1)
    {
        return 1;
    }

    if (setup_server_socket(server_socket, &server_address, PORT) == -1)
    {
        close(server_socket);
        return 1;
    }

    printf("Server bound to port %d.\n", PORT);

    if (start_server(server_socket) == -1)
    {
        close(server_socket);
        return 1;
    }

    printf("Server is listening...\n");

    while (1)
    {
        int client_socket = accept_client(server_socket);
        if (client_socket == -1)
        {
            continue;
        }

        printf("Client connected!\n");

        int *client_socket_ptr = malloc(sizeof(int));
        if (client_socket_ptr == NULL)
        {
            close(client_socket);
            continue;
        }
        *client_socket_ptr = client_socket;

        pthread_t thread;
        if (pthread_create(&thread, NULL, handle_client, client_socket_ptr) != 0)
        {
            perror("pthread_create");
            free(client_socket_ptr);
            close(client_socket);
            continue;
        }
        pthread_detach(thread);
    }

    close(server_socket);

    return 0;
}
