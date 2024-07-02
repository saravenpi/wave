#include "wave.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
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

int check_path(const char *path, int client_fd)
{
    struct stat path_stat;

    if (strstr(path, "..")) {
        send_response(client_fd, "Forbidden", FORBIDDEN);
        return 1;
    }
    if (access(path, F_OK) == -1) {
        send_response(client_fd, "Not Found", NOT_FOUND);
        return 1;
    }
    if (access(path, R_OK) == -1) {
        send_response(client_fd, "Forbidden", FORBIDDEN);
        return 1;
    }
    int status = stat(path, &path_stat);
    if (status == -1) {
        send_response(
            client_fd, "Internal Server Error", INTERNAL_SERVER_ERROR);
        return 1;
    }
    if (S_ISDIR(path_stat.st_mode)) {
        send_response(client_fd, "Forbidden", FORBIDDEN);
        return 1;
    }
    return 0;
}

void send_file_response(int client_fd, const char *path)
{
    FILE *file;
    char response[BUFFER_SIZE];
    size_t bytes_read;
    char buffer[BUFFER_SIZE];
    char *content_type;
    char *real_path;

    if (path[0] == '/')
        path++;
    content_type = get_content_type(path);
    real_path = malloc(strlen("static/") + strlen(path) + 1);
    if (!real_path) {
        perror("[WAVE] Failed to allocate memory for real_path");
        return send_response(
            client_fd, "Internal Server Error", INTERNAL_SERVER_ERROR);
    }
    snprintf(
        real_path, strlen("static/") + strlen(path) + 1, "static/%s", path);
    if (check_path(real_path, client_fd))
        return;
    printf("real_path: %s\n", real_path);
    file = fopen(real_path, "rb");
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

void send_response(int client_fd, const char *body, int status_code)
{
    char response[BUFFER_SIZE];
    ssize_t result;

    if (status_code < 100 || status_code >= 600) {
        fprintf(stderr, "[WAVE] Invalid status code: %d\n", status_code);
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
        status_code, strlen(body), body);
    result = write(client_fd, response, strlen(response));
    if (result == -1) {
        perror("[WAVE] Failed to send response");
    }
}
