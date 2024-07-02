#include "wave.h"
#include <stdio.h>
#include <string.h>

void add_route(server_t *server, const char *method, const char *path,
    request_handler_t handler)
{
    if (server->route_count < MAX_ROUTES) {
        server->routes[server->route_count].method = strdup(method);
        server->routes[server->route_count].path = strdup(path);
        server->routes[server->route_count].handler = handler;
        server->route_count++;
    } else {
        fprintf(stderr, "[WAVE] Maximum number of endpoints reached\n");
    }
}

void find_route_and_handle(
    server_t *server, int client_fd, const char *request)
{
    char *request_line = strtok(strdup(request), "\r\n");
    char *path = NULL;
    char *method = NULL;

    if (!request_line)
        return send_response(client_fd, "Bad Request", BAD_REQUEST);
    method = strtok(request_line, " ");
    path = strtok(NULL, " ");
    if (!path || !method)
        return send_response(client_fd, "Bad Request", BAD_REQUEST);
    printf("[WAVE] Request: %s %s\n", method, path);
    for (int i = 0; i < server->route_count; i++) {
        if (strcmp(path, server->routes[i].path) != 0)
            continue;
        if (strcmp(method, server->routes[i].method) != 0) {
            return send_response(
                client_fd, "Method Not Allowed", METHOD_NOT_ALLOWED);
        }
        return server->routes[i].handler(client_fd, request);
    }
    return send_file_response(client_fd, path);
}

void default_handler(int client_fd, const char *request)
{
    send_response(client_fd, "Hello, World!", OK);
}
