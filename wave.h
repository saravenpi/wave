#ifndef WAVE_H
#define WAVE_H

#include <netinet/in.h>
#define BUFFER_SIZE 4096
#define MAX_CLIENTS 100
#define MAX_ROUTES 50

typedef enum {
    OK = 200,
    BAD_REQUEST = 400,
    NOT_FOUND = 404,
    INTERNAL_SERVER_ERROR = 500,
    METHOD_NOT_ALLOWED = 405,
    FORBIDDEN = 403
} http_status_t;

typedef void (*request_handler_t)(int client_fd, const char *request);

typedef struct {
    char *path;
    char *method;
    request_handler_t handler;
} route_t;

typedef struct {
    int fd;
    int max_fd;
    int clients[MAX_CLIENTS];
    char buffers[MAX_CLIENTS][BUFFER_SIZE];
    struct sockaddr_in address;
    fd_set read_fd_set;
    int addrlen;
    route_t routes[MAX_ROUTES];
    int route_count;
} server_t;

typedef struct {
    char *method;
    char *path;
    char *version;
    char *headers;
    char *body;
} request_t;

server_t *init_server(int port);
void start_server(server_t *server);
void default_handler(int clientFd, const char *request);
void add_route(server_t *server, const char *method, const char *path,
    request_handler_t handler);
request_t *parse_request(const char *request);
void send_response(int client_fd, const char *body, int status_code);
void send_file_response(int clientFd, const char *path);
#endif
