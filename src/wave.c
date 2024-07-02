#include "wave.h"
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

void add_sockets_to_set(server_t *server);
void check_new_connections(server_t *server);
void handle_client_requests(server_t *server);

server_t *init_server(int port)
{
    server_t *server = (server_t *)malloc(sizeof(server_t));
    if (!server) {
        perror("[WAVE] Failed to allocate memory for server");
        exit(EXIT_FAILURE);
    }

    server->addrlen = sizeof(struct sockaddr_in);
    if ((server->fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("[WAVE] Socket creation failed");
        free(server);
        exit(EXIT_FAILURE);
    }
    server->address.sin_family = AF_INET;
    server->address.sin_addr.s_addr = INADDR_ANY;
    server->address.sin_port = htons(port);

    if (bind(server->fd, (struct sockaddr *)&server->address,
            server->addrlen) < 0) {
        perror("[WAVE] Bind failed");
        close(server->fd);
        free(server);
        exit(EXIT_FAILURE);
    }

    if (listen(server->fd, 3) < 0) {
        perror("[WAVE] Listen failed");
        close(server->fd);
        free(server);
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < MAX_CLIENTS; i++) {
        server->clients[i] = 0;
        memset(server->buffers[i], 0, BUFFER_SIZE);
    }
    server->max_fd = server->fd;
    server->route_count = 0;
    return server;
}

void start_server(server_t *server)
{
    printf("🌊 Wave server started !\n");
    printf("[WAVE] server_t port: %d\n", ntohs(server->address.sin_port));
    printf("[WAVE] server_t host: %s\n", inet_ntoa(server->address.sin_addr));
    while (1) {
        add_sockets_to_set(server);
        int activity =
            select(server->max_fd + 1, &server->read_fd_set, NULL, NULL, NULL);
        if (activity < 0) {
            perror("[WAVE] Select failed");
            break;
        }
        if (FD_ISSET(server->fd, &server->read_fd_set)) {
            check_new_connections(server);
        }
        handle_client_requests(server);
    }
    printf("[WAVE] server_t shutting down\n");
}
