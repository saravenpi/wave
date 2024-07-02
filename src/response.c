#include "wave.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>

char *getContentType(const char *path)
{
    if (strstr(path, ".css"))
        return "text/css";
    if (strstr(path, ".js"))
        return "application/javascript";
    if (strstr(path, ".png"))
        return "image/png";
    if (strstr(path, ".jpg") || strstr(path, ".jpeg"))
        return "image/jpeg";
    if (strstr(path, ".gif"))
        return "image/gif";
    if (strstr(path, ".mp4"))
        return "video/mp4";
    if (strstr(path, ".pdf"))
        return "application/pdf";
    if (strstr(path, ".html") || strstr(path, ".htm"))
        return "text/html";
    return "application/octet-stream";
}

void sendFileResponse(int clientFd, const char *path)
{
    FILE *file;
    char response[BUFFER_SIZE];
    size_t bytesRead;
    char buffer[BUFFER_SIZE];
    char *contentType;

    contentType = getContentType(path);
    file = fopen(path, "rb");
    if (!file) {
        perror("[WAVE] Failed to open file");
        return sendResponse(clientFd, "Not Found", NOT_FOUND);
    }
    snprintf(response, sizeof(response),
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: %s\r\n"
        "Connection: close\r\n"
        "\r\n",
        contentType);
    write(clientFd, response, strlen(response));

    while ((bytesRead = fread(buffer, 1, sizeof(buffer), file)) > 0) {
        write(clientFd, buffer, bytesRead);
    }
    fclose(file);
}

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
