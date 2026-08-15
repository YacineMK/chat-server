#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <netinet/in.h>

#include "network.h"
#include "log.h"

#define PORT 8080
#define LOG_PATH "logs/server.log"

int main(void)
{
    int server_socket;
    struct sockaddr_in server_address;

    if (log_init(LOG_PATH) == -1)
    {
        fprintf(stderr, "failed to open log file %s\n", LOG_PATH);
        return 1;
    }

    log_message("INFO", "server starting on port %d", PORT);

    server_socket = create_socket();
    if (server_socket == -1)
    {
        log_message("ERROR", "socket creation failed");
        log_close();
        return 1;
    }

    if (setup_server_socket(server_socket, &server_address, PORT) == -1)
    {
        log_message("ERROR", "bind to port %d failed", PORT);
        close(server_socket);
        log_close();
        return 1;
    }

    printf("Server bound to port %d.\n", PORT);
    log_message("INFO", "bound to port %d", PORT);

    if (start_server(server_socket) == -1)
    {
        log_message("ERROR", "listen failed");
        close(server_socket);
        log_close();
        return 1;
    }

    printf("Server is listening...\n");
    log_message("INFO", "listening for clients");

    while (1)
    {
        int client_socket = accept_client(server_socket);
        if (client_socket == -1)
        {
            log_message("WARN", "accept failed");
            continue;
        }

        printf("Client connected!\n");
        log_message("INFO", "client connected fd=%d", client_socket);

        int *client_socket_ptr = malloc(sizeof(int));
        if (client_socket_ptr == NULL)
        {
            log_message("ERROR", "malloc failed for client fd=%d", client_socket);
            close(client_socket);
            continue;
        }
        *client_socket_ptr = client_socket;

        pthread_t thread;
        if (pthread_create(&thread, NULL, handle_client, client_socket_ptr) != 0)
        {
            log_message("ERROR", "pthread_create failed for client fd=%d", client_socket);
            perror("pthread_create");
            free(client_socket_ptr);
            close(client_socket);
            continue;
        }
        pthread_detach(thread);
    }

    close(server_socket);
    log_close();

    return 0;
}
