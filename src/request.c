#include "wave.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
