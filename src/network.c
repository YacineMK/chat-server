#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "network.h"
#include "log.h"

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

int setup_server_socket(int server_socket, struct sockaddr_in *server_address, int port) {
    server_address->sin_family = AF_INET;
    server_address->sin_port = htons(port);
    server_address->sin_addr.s_addr = INADDR_ANY;

    if (bind(server_socket, (struct sockaddr *)server_address, sizeof(*server_address)) < 0)
    {
        perror("Bind failed");
        return -1;
    }
    return 0;
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
    client_socket = accept(server_socket, NULL, NULL);

    if (client_socket == -1)
    {
        perror("accept");
        return -1;
    }
    return client_socket;
}

int connect_to_server(int client_socket, const char *host, int port) {
    struct sockaddr_in server_address;

    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port);

    if (inet_pton(AF_INET, host, &server_address.sin_addr) <= 0)
    {
        perror("inet_pton");
        return -1;
    }

    if (connect(client_socket, (struct sockaddr *)&server_address, sizeof(server_address)) < 0)
    {
        perror("connect");
        return -1;
    }
    return 0;
}

static int read_all(int socket_fd, void *buffer, size_t length) {
    size_t total = 0;
    char *buf = buffer;

    while (total < length)
    {
        ssize_t n = recv(socket_fd, buf + total, length - total, 0);
        if (n == 0)
        {
            return 0;
        }
        if (n < 0)
        {
            return -1;
        }
        total += (size_t)n;
    }
    return 1;
}

static int write_all(int socket_fd, const void *buffer, size_t length) {
    size_t total = 0;
    const char *buf = buffer;

    while (total < length)
    {
        ssize_t n = send(socket_fd, buf + total, length - total, 0);
        if (n <= 0)
        {
            return -1;
        }
        total += (size_t)n;
    }
    return 0;
}

int send_message(int socket_fd, const char *message) {
    if (message == NULL)
    {
        return -1;
    }

    size_t length = strlen(message);
    if (length > 0xFFFFFFFFu)
    {
        return -1;
    }

    uint32_t net_length = htonl((uint32_t)length);

    if (write_all(socket_fd, &net_length, sizeof(net_length)) < 0)
    {
        perror("send");
        return -1;
    }

    if (length > 0 && write_all(socket_fd, message, length) < 0)
    {
        perror("send");
        return -1;
    }

    return (int)length;
}

int receive_message(int socket_fd, char **message) {
    uint32_t net_length;
    int rc = read_all(socket_fd, &net_length, sizeof(net_length));
    if (rc <= 0)
    {
        return rc;
    }

    uint32_t length = ntohl(net_length);

    char *buffer = malloc((size_t)length + 1);
    if (buffer == NULL)
    {
        return -1;
    }

    if (length > 0)
    {
        rc = read_all(socket_fd, buffer, length);
        if (rc <= 0)
        {
            free(buffer);
            return rc;
        }
    }

    buffer[length] = '\0';
    *message = buffer;
    return (int)length;
}

void *handle_client(void *arg)
{
    int client_socket = *(int *)arg;

    free(arg);

    log_message("INFO", "client fd=%d handler started", client_socket);

    while (1)
    {
        char *message = NULL;

        if (receive_message(client_socket, &message) <= 0)
        {
            log_message("INFO", "client fd=%d disconnected", client_socket);
            break;
        }

        printf("Client: %s\n", message);
        log_message("INFO", "recv fd=%d: %s", client_socket, message);

        const char *reply = "Message received";
        send_message(client_socket, reply);
        log_message("INFO", "send fd=%d: %s", client_socket, reply);

        free(message);
    }

    close(client_socket);

    return NULL;
}
