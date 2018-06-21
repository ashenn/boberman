#include "../server.h"

server_t* socket_init(unsigned int addr, int port) {
    Game* game = getGame();
    logger->enabled = game->flags & DBG_SERVER;
    logger->inf("==== INIT SOCKET ====");    
    logger->dbg("-- addr: %d:%d", addr, port);    

    int socket_fd;
    struct sockaddr_in server;
    server_t* response = malloc(sizeof(server_t));

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = addr;
    server.sin_port = htons(port);

    socket_fd = socket(AF_INET , SOCK_STREAM , 0);
    if (socket_fd == -1)
    {
        logger->err("Could not create socket");
    }

    logger->dbg("-- Master socket created");

    if( bind(socket_fd,(struct sockaddr *)&server , sizeof(server)) < 0)
    {
        logger->err("Socket Bind failed..");
        return response;
    }
    char m[30];
    logger->dbg("-- Socket Bind successful on port : %d", port);

    listen(socket_fd, 3);

    response->fd = socket_fd;
    response->addr = server;

    return response;
}