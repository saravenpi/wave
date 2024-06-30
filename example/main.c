#include "wave.h"

void aboutHandler(int clientFd, const char *request)
{
    sendResponse(clientFd, "This is a custom endpoint!", 200);
}

void customHandler(int clientFd, const char *request)
{
    sendFileResponse(clientFd, "static/index.html");
}

int main(void)
{
    Server *server = initServer(8081);

    addEndpoint(server, "GET", "/", defaultHandler);
    addEndpoint(server, "GET", "/about", aboutHandler);
    addEndpoint(server, "GET", "/custom", customHandler);
    startWebServer(server);
    return 0;
}
