#include "wave.h"
#include <stdio.h>
#include <string.h>

void addEndpoint(Server *server, const char *method, const char *path,
    RequestHandler handler)
{
    if (server->endpointCount < MAX_ENDPOINTS) {
        server->endpoints[server->endpointCount].method = strdup(method);
        server->endpoints[server->endpointCount].path = strdup(path);
        server->endpoints[server->endpointCount].handler = handler;
        server->endpointCount++;
    } else {
        fprintf(stderr, "[WAVE] Maximum number of endpoints reached\n");
    }
}

void findEndpointAndHandle(Server *server, int clientFd, const char *request)
{
    char *requestLine = strtok(strdup(request), "\r\n");
    char *path = NULL;
    char *method = NULL;

    if (!requestLine)
        return sendResponse(clientFd, "Bad Request", BAD_REQUEST);
    method = strtok(requestLine, " ");
    path = strtok(NULL, " ");
    if (!path || !method)
        return sendResponse(clientFd, "Bad Request", BAD_REQUEST);
    printf("[WAVE] Request: %s %s\n", method, path);
    for (int i = 0; i < server->endpointCount; i++) {
        if (strcmp(path, server->endpoints[i].path) != 0)
            continue;
        if (strcmp(method, server->endpoints[i].method) != 0) {
            return sendResponse(
                clientFd, "Method Not Allowed", METHOD_NOT_ALLOWED);
        }
        return server->endpoints[i].handler(clientFd, request);
    }
    return sendFileResponse(clientFd, path + 1);
}

void defaultHandler(int clientFd, const char *request)
{
    sendResponse(clientFd, "Hello, World!", OK);
}
