#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "network.h"

#define SERVER_HOST "127.0.0.1"
#define PORT 8080
#define INPUT_BUFFER_SIZE 1024

int main(void)
{
    int client_socket = create_socket();
    if (client_socket == -1)
    {
        return 1;
    }

    if (connect_to_server(client_socket, SERVER_HOST, PORT) == -1)
    {
        close(client_socket);
        return 1;
    }

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
            break;
        }

        char *reply = NULL;
        if (receive_message(client_socket, &reply) <= 0)
        {
            break;
        }

        printf("Server: %s\n", reply);
        free(reply);
    }

    close(client_socket);
    return 0;
}
