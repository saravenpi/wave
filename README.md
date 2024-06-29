# 🌊 Wave
## About
Wave is a simple C http webserver library

## Example
Here is a code example of the wave library
```c
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
```

Now compile your code with:
```bash
gcc main.c -I. -lwave -L. -Wl,-rpath,.
```
