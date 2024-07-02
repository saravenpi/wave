#include "wave.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>

char *get_content_type(const char *path)
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

void send_file_response(int client_fd, const char *path)
{
    FILE *file;
    char response[BUFFER_SIZE];
    size_t bytes_read;
    char buffer[BUFFER_SIZE];
    char *content_type;

    content_type = get_content_type(path);
    file = fopen(path, "rb");
    if (!file) {
        perror("[WAVE] Failed to open file");
        return send_response(client_fd, "Not Found", NOT_FOUND);
    }
    snprintf(response, sizeof(response),
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: %s\r\n"
        "Connection: close\r\n"
        "\r\n",
        content_type);
    write(client_fd, response, strlen(response));

    while ((bytes_read = fread(buffer, 1, sizeof(buffer), file)) > 0) {
        write(client_fd, buffer, bytes_read);
    }
    fclose(file);
}

void send_response(int client_fd, const char *body, int statusCode)
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
    result = write(client_fd, response, strlen(response));
    if (result == -1) {
        perror("[WAVE] Failed to send response");
    }
}
