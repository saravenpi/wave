#include "wave.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>

void findEndpointAndHandle(Server *server, int clientFd, const char *request);

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

void processClientData(Server *server, int clientFd, int index)
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
        return disconnectClient(server, clientFd);
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
            processClientData(server, server->clients[i], i);
        }
    }
}
