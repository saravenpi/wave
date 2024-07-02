# 🌊 Wave, the simple HTTP server library for C
## About
Wave is a simple C HTTP server library. You can use it to create your backend in C. It supports serving static files.

<img alt="Welcome to Wave" src="https://vhs.charm.sh/vhs-5NpF84KkSDRpi9GRSHPxbk.gif" width="600" />

## How to use it ?

### Prerequisites
- First you need to compile the library and add the `libwave.so` file in your codebase.
- After that copy the `wave.h` header file of the library in your codebase.

Now the wave library is ready to be used!

### Example webserver implementation

Here is an example implementation using the wave library:
```c
#include "wave.h"

void about_handler(int client_fd, const char *request)
{
    send_response(client_fd, "This is a custom endpoint!", 200);
}

void custom_handler(int client_fd, const char *request)
{
    send_file_response(client_fd, "static/index.html");
}

int main(void)
{
   server_t  *server = init_server(8080);

    add_route(server, "GET", "/", default_handler);
    add_route(server, "GET", "/about", about_handler);
    add_route(server, "GET", "/custom", custom_handler);
    start_server(server);
    return 0;
}

```

You can compile your webserver with:
```bash
gcc main.c -I. -lwave -L. -Wl,-rpath,.
```

🎉 And your webserver now ready to go !
