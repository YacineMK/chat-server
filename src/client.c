#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "network.h"
#include "log.h"

#define SERVER_HOST "127.0.0.1"
#define PORT 8080
#define INPUT_BUFFER_SIZE 1024
#define LOG_PATH "logs/client.log"

int main(void)
{
    if (log_init(LOG_PATH) == -1)
    {
        fprintf(stderr, "failed to open log file %s\n", LOG_PATH);
        return 1;
    }

    log_message("INFO", "client starting, target %s:%d", SERVER_HOST, PORT);

    int client_socket = create_socket();
    if (client_socket == -1)
    {
        log_message("ERROR", "socket creation failed");
        log_close();
        return 1;
    }

    if (connect_to_server(client_socket, SERVER_HOST, PORT) == -1)
    {
        log_message("ERROR", "connect to %s:%d failed", SERVER_HOST, PORT);
        close(client_socket);
        log_close();
        return 1;
    }

    log_message("INFO", "connected to %s:%d fd=%d", SERVER_HOST, PORT, client_socket);
    printf("Connected to %s:%d. Type messages (empty line to quit).\n",
           SERVER_HOST, PORT);

    char input[INPUT_BUFFER_SIZE];

    while (fgets(input, sizeof(input), stdin) != NULL)
    {
        size_t len = strlen(input);
        if (len > 0 && input[len - 1] == '\n')
        {
            input[len - 1] = '\0';
            len--;
        }

        if (len == 0)
        {
            break;
        }

        if (send_message(client_socket, input) < 0)
        {
            log_message("ERROR", "send failed");
            break;
        }
        log_message("INFO", "sent: %s", input);

        char *reply = NULL;
        if (receive_message(client_socket, &reply) <= 0)
        {
            log_message("WARN", "server closed connection");
            break;
        }

        printf("Server: %s\n", reply);
        log_message("INFO", "recv: %s", reply);
        free(reply);
    }

    log_message("INFO", "client shutting down");
    close(client_socket);
    log_close();
    return 0;
}
