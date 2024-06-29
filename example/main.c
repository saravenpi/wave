#include "wave.h"

void customHandler(int clientFd, const char *request)
{
    sendResponse(clientFd, "This is a custom endpoint!", 200);
}

int main(int argc, char *argv[])
{
    Server *server = initServer(8080);

    addEndpoint(server, "GET", "/", defaultHandler);
    addEndpoint(server, "GET", "/custom", customHandler);
    startWebServer(server);
    return 0;
}
