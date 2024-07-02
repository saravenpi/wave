
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

    // Parse request line
    char *requestLine = strtok(strdup(request), "\r\n");
    if (requestLine) {
        req->method = strdup(strtok(requestLine, " "));
        req->path = strdup(strtok(NULL, " "));
        req->version = strdup(strtok(NULL, " "));
        free(requestLine);
    } else {
        free(req);
        return NULL;
    }

    // Parse headers
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

    // Parse body
    char *body = strstr(request, "\r\n\r\n");
    if (body) {
        body += 4;
        req->body = strdup(body);
    }

    return req;
}
