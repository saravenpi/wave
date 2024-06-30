# 🌊 Wave
## About
Wave is a simple C http webserver library. You can use it to create your backend in C. It supports serving static files.

<img alt="Welcome to Wave" src="https://vhs.charm.sh/vhs-5NpF84KkSDRpi9GRSHPxbk.gif" width="600" />

## Example

Here is an example to understand how to use the wave library:
```c
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
```

You can compile your webserver with:
```bash
gcc main.c -I. -lwave -L. -Wl,-rpath,.
```

🎉 And you're now ready to go!
