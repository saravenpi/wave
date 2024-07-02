#include "wave.h"
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

void addSocketsToSet(Server *server);
void checkNewConnections(Server *server);
void handleClientRequests(Server *server);

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

void startWebServer(Server *server)
{
    printf("🌊 Wave server started !\n");
    printf("[WAVE] Server port: %d\n", ntohs(server->address.sin_port));
    printf("[WAVE] Server host: %s\n", inet_ntoa(server->address.sin_addr));
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
