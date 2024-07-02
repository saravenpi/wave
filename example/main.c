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
