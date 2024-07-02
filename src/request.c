#include "wave.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Parse request and return Request object
request_t *parse_request(const char *request)
{
    request_t *req = (request_t *)malloc(sizeof(request_t));
    if (!req) {
        perror("[WAVE] Failed to allocate memory for request");
        return NULL;
    }

    // Parse request line
    char *request_line = strtok(strdup(request), "\r\n");
    if (request_line) {
        req->method = strdup(strtok(request_line, " "));
        req->path = strdup(strtok(NULL, " "));
        req->version = strdup(strtok(NULL, " "));
        free(request_line);
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
