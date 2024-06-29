#include "wave.h"
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

void sendResponse(int clientFd, const char *body, int statusCode)
{
    char response[BUFFER_SIZE];
    ssize_t result;

    if (statusCode < 100 || statusCode >= 600) {
        fprintf(stderr, "[WAVE] Invalid status code: %d\n", statusCode);
        return;
    }
    if (strlen(body) >= BUFFER_SIZE) {
        fprintf(stderr, "[WAVE] Response body is too large\n");
        return;
    }
    snprintf(response, sizeof(response),
        "HTTP/1.1 %d OK\r\n"
        "Content-Type: text/plain\r\n"
        "Content-Length: %zu\r\n"
        "\r\n"
        "%s",
        statusCode, strlen(body), body);
    result = write(clientFd, response, strlen(response));
    if (result == -1) {
        perror("[WAVE] Failed to send response");
    }
}

Server *initServer(int port)
{
    Server *server = (Server *)malloc(sizeof(Server));
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
    server->maxFd = server->fd;
    server->endpointCount = 0;
    return server;
}

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
    return sendResponse(clientFd, "Not Found", NOT_FOUND);
}

void disconnectClient(Server *server, int clientFd)
{
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (server->clients[i] == clientFd) {
            close(clientFd);
            server->clients[i] = 0;
            memset(server->buffers[i], 0, BUFFER_SIZE);
            break;
        }
    }
}

void handleClient(Server *server, int clientFd, int index)
{
    int valread;
    char buffer[BUFFER_SIZE];
    int requestComplete;
    memset(buffer, 0, BUFFER_SIZE);

    valread = read(clientFd, buffer, BUFFER_SIZE - 1);
    if (valread == 0) {
        return disconnectClient(server, clientFd);
    }
    if (valread < 0) {
        perror("[WAVE] Read failed");
        return disconnectClient(server, clientFd);
    }
    buffer[valread] = '\0';
    strncat(server->buffers[index], buffer,
        BUFFER_SIZE - strlen(server->buffers[index]) - 1);
    requestComplete = strstr(server->buffers[index], "\r\n\r\n") != NULL;
    if (requestComplete) {
        findEndpointAndHandle(server, clientFd, server->buffers[index]);
        close(clientFd);
        server->clients[index] = 0;
        memset(server->buffers[index], 0, BUFFER_SIZE);
    }
}

void checkNewConnections(Server *server)
{
    int newSocket;
    if ((newSocket = accept(server->fd, (struct sockaddr *)&server->address,
             (socklen_t *)&server->addrlen)) < 0) {
        perror("[WAVE] Accept failed");
        return;
    }
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (server->clients[i] == 0) {
            server->clients[i] = newSocket;
            break;
        }
    }
}

void addSocketsToSet(Server *server)
{
    FD_ZERO(&server->readFdSet);
    FD_SET(server->fd, &server->readFdSet);
    server->maxFd = server->fd;
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (server->clients[i] > 0) {
            FD_SET(server->clients[i], &server->readFdSet);
            if (server->clients[i] > server->maxFd) {
                server->maxFd = server->clients[i];
            }
        }
    }
}

void handleClientRequests(Server *server)
{
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (server->clients[i] > 0 &&
            FD_ISSET(server->clients[i], &server->readFdSet)) {
            handleClient(server, server->clients[i], i);
        }
    }
}

void startWebServer(Server *server)
{
    printf("🌊 Wave - A simple C web server\n");
    printf(
        "[WAVE] Server started on port %d\n", ntohs(server->address.sin_port));
    printf("[WAVE] Server host is %s\n", inet_ntoa(server->address.sin_addr));
    while (1) {
        addSocketsToSet(server);
        int activity =
            select(server->maxFd + 1, &server->readFdSet, NULL, NULL, NULL);
        if (activity < 0) {
            perror("[WAVE] Select failed");
            break;
        }
        if (FD_ISSET(server->fd, &server->readFdSet)) {
            checkNewConnections(server);
        }
        handleClientRequests(server);
    }
    printf("[WAVE] Server shutting down\n");
}

void defaultHandler(int clientFd, const char *request)
{
    sendResponse(clientFd, "Hello, World!", OK);
}

Request *parseRequest(const char *request)
{
    Request *req = (Request *)malloc(sizeof(Request));

    if (!req) {
        perror("[WAVE] Failed to allocate memory for request");
        return NULL;
    }
    char *requestLine = strtok(strdup(request), "\r\n");
    if (requestLine) {
        char *method = strtok(requestLine, " ");
        char *path = strtok(NULL, " ");
        char *version = strtok(NULL, " ");
        if (method && path && version) {
            req->method = strdup(method);
            req->path = strdup(path);
            req->version = strdup(version);
        }
    } else {
        free(req);
        return NULL;
    }
    char *headers = strstr(request, "\r\n");
    if (headers) {
        headers += 2;
        char *end = strstr(headers, "\r\n\r\n");
        if (end) {
            size_t len = end - headers;
            req->headers = (char *)malloc(len + 1);
            if (req->headers) {
                strncpy(req->headers, headers, len);
                req->headers[len] = '\0';
            }
        }
    }
    if (req->headers && strlen(req->headers) == 0) {
        free(req->headers);
        req->headers = NULL;
    }
    char *body = strstr(request, "\r\n\r\n");
    if (body) {
        body += 4;
        req->body = strdup(body);
    }
    if (req->body && strlen(req->body) == 0) {
        free(req->body);
        req->body = NULL;
    }
    return req;
}
