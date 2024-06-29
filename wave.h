#ifndef WAVE_H
#define WAVE_H

#include <netinet/in.h>
#define BUFFER_SIZE 4096
#define MAX_CLIENTS 100
#define MAX_ENDPOINTS 50

enum httpStatus {
    OK = 200,
    BAD_REQUEST = 400,
    NOT_FOUND = 404,
    INTERNAL_SERVER_ERROR = 500,
    METHOD_NOT_ALLOWED = 405,
};
typedef enum httpStatus httpStatus;

struct server;
typedef void (*RequestHandler)(int clientFd, const char *request);

typedef struct {
    char *path;
    char *method;
    RequestHandler handler;
} Endpoint;

struct server {
    int fd;
    int maxFd;
    int clients[MAX_CLIENTS];
    char buffers[MAX_CLIENTS][BUFFER_SIZE];
    struct sockaddr_in address;
    fd_set readFdSet;
    int addrlen;
    Endpoint endpoints[MAX_ENDPOINTS];
    int endpointCount;
};
typedef struct server Server;

struct request {
    char *method;
    char *path;
    char *version;
    char *headers;
    char *body;
};
typedef struct request Request;

Server *initServer(int port);
void startWebServer(Server *server);
void defaultHandler(int clientFd, const char *request);
void registerEndpoint(Server *server, const char *method, const char *path,
    RequestHandler handler);
void sendHTTPResponse(int clientFd, const char *body, int statusCode);
Request *parseRequest(const char *request);
#endif
