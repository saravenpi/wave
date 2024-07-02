#include "wave.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>

void find_route_and_handle(
    server_t *server, int client_fd, const char *request);

void disconnect_client(server_t *server, int client_fd)
{
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (server->clients[i] == client_fd) {
            close(client_fd);
            server->clients[i] = 0;
            memset(server->buffers[i], 0, BUFFER_SIZE);
            break;
        }
    }
}

void process_client_data(server_t *server, int client_fd, int index)
{
    int valread;
    char buffer[BUFFER_SIZE];
    int request_complete;
    memset(buffer, 0, BUFFER_SIZE);

    valread = read(client_fd, buffer, BUFFER_SIZE - 1);
    if (valread == 0) {
        return disconnect_client(server, client_fd);
    }
    if (valread < 0) {
        perror("[WAVE] Read failed");
        return disconnect_client(server, client_fd);
    }
    buffer[valread] = '\0';
    strncat(server->buffers[index], buffer,
        BUFFER_SIZE - strlen(server->buffers[index]) - 1);
    request_complete = strstr(server->buffers[index], "\r\n\r\n") != NULL;
    if (request_complete) {
        find_route_and_handle(server, client_fd, server->buffers[index]);
        return disconnect_client(server, client_fd);
    }
}

void check_new_connections(server_t *server)
{
    int new_socket;
    if ((new_socket = accept(server->fd, (struct sockaddr *)&server->address,
             (socklen_t *)&server->addrlen)) < 0) {
        perror("[WAVE] Accept failed");
        return;
    }
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (server->clients[i] == 0) {
            server->clients[i] = new_socket;
            break;
        }
    }
}

void add_sockets_to_set(server_t *server)
{
    FD_ZERO(&server->read_fd_set);
    FD_SET(server->fd, &server->read_fd_set);
    server->max_fd = server->fd;
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (server->clients[i] > 0) {
            FD_SET(server->clients[i], &server->read_fd_set);
            if (server->clients[i] > server->max_fd) {
                server->max_fd = server->clients[i];
            }
        }
    }
}

void handle_client_requests(server_t *server)
{
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (server->clients[i] > 0 &&
            FD_ISSET(server->clients[i], &server->read_fd_set)) {
            process_client_data(server, server->clients[i], i);
        }
    }
}
