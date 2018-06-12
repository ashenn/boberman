#include "../../libs/libmy_extended.h"
#include "../server.h"

server_t socket_init(unsigned int addr, int port) {
    

    int socket_fd;
    struct sockaddr_in server;
    server_t response;

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = addr;
    server.sin_port = htons(port);

    socket_fd = socket(AF_INET , SOCK_STREAM , 0);
    if (socket_fd == -1)
    {
        error(logger, "Could not create socket");
    }
    info(logger, "Master socket created ...\n");

    if( bind(socket_fd,(struct sockaddr *)&server , sizeof(server)) < 0)
    {
        error(logger, "Bind failed..");
        return response;
    }
    char m[30];
    my_strcat(m, "Bind successful on port : ");
    my_strcat(m, (char *)opts->next->value);
    info(logger, m);

    listen(socket_fd , 3);

    response.fd = socket_fd;
    response.addr = server;

    return response;
}