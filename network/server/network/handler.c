#include "../server.h"

int add_child_socket_to_set(int max_sd, int *client_socket, fd_set *readfds) {
    int sd;
    for (int i = 0 ; i < 4 ; i++)
    {
        sd = *(client_socket + i);

        if(sd > 0)
            FD_SET( sd , readfds);

        if(sd > max_sd)
            max_sd = sd;
    }
    return max_sd;
}

char *tamer (char *b) {
    char *c = malloc(strlen(b) + 1);
    c[0] = 0x03;
    for (int i =0; i < strlen(b); i++)
        c[i+1] = b[i];
    return c;
}


void handle_master_socket(int master_socket_fd, int *client_socket, struct sockaddr_in server, fd_set *readfds) {
    Game* game = getGame();
    server_t* serv = getServer();

    logger->enabled = game->flags & DBG_SERVER;
    logger->inf("==== HANDLE MASTER SOCKET ====");

    if (!FD_ISSET(master_socket_fd, readfds)) {
        return;
    }

    int new_socket;
    int addrlen = sizeof(&server);

    if ((new_socket = accept(master_socket_fd, (struct sockaddr *)&server, (socklen_t*)&addrlen))<0) {
        logger->err("Faild To Accept New Socket !!!");
        exit(EXIT_FAILURE);
    }

    logger->inf("New connection !!");
    if(serv->clients->nodeCount >= 4) {
        char motd[6];
        snprintf(motd, 6, "Fail:0");
        logger->dbg("Send Message: %s", motd);


        if( send(new_socket, motd, strlen(motd), 0) != strlen(motd) ) {
            logger->err("Faild To Send Message");
        }

        return;
    }


    logger->inf("Adding New Player: %d", serv->clients->nodeCount+1);

    char clientName[12];
    snprintf(clientName, 12, "Player-%d", serv->clients->nodeCount+1);
    logger->dbg("-- Name: %s", clientName);

    client_t* client = malloc(sizeof(client_t));

    Node* cliNode = addNodeV(serv->clients, clientName, client, 1);

    client->id = cliNode->id;

    logger->dbg("-- id: %d", client->id);

    char motd[43];
    snprintf(motd, 43, " ok:%d", client->id);
    motd[0] = 4;

    logger->dbg("Send Message: %s", motd);

    if( send(new_socket, motd, strlen(motd), 0) != strlen(motd) ) {
        logger->err("Faild To Send Welcome Message");
    }
    char newPlayer[43];
    snprintf(newPlayer, 43, "newPlayer:%d", client->id);


    for (int i = 0; i < 4; i++) {
        if( *(client_socket + i) == 0 ) {
            *(client_socket + i) = new_socket;
            logger->inf("Adding to list of sockets as %d\n" , i);
            break;
        }
    }

    //logger->war("HANDLER: Ask-Lock");
    //lock(DBG_SERVER);
    //logger->war("HANDLER: Lock");

    client->name = malloc(strlen(clientName)+1);
    memset(client->name, 0, strlen(clientName)+1);
    strcpy(client->name, clientName);

    client->fd = new_socket;
    client->player = genPlayer(clientName);
    if(serv->clients->nodeCount == 1) {
        initPlayer(client->player);
    }

    broadcast(newPlayer, getClient());
    
    if(serv->clients->nodeCount == 2) {
        sleep(1);
        changeGameStatus(GAME_START);
    }
    //logger->war("HANDLER: Un-Lock");
    //unlock(DBG_SERVER);

    logger->dbg("==== HANDLE MASTER SOCKET END ====");
}

int find_client_by_fd(int fd) {
    printf("all : {");
    Node *tmp = NULL;
    server_t* server = getServer();

    int i = 0;
    while((tmp = listIterate(server->clients, tmp)) != NULL) {
        client_t *client = tmp->value;

        if (client->fd != -1) {
            if (client->fd == fd){
                return i;
            }
        }
        i++;
    }

    return -1;
}

int find_client_by_id(int id) {
    Node *tmp = NULL;
    server_t* server = getServer();

    int i = 0;
    while((tmp = listIterate(server->clients, tmp)) != NULL) {
        client_t *client = tmp->value;
        if (client->id == id) {
            return i;
        }
        i++;
    }

    return -1;
}

client_t* getClientByFd(int fd) {
    Node *tmp = NULL;
    server_t* server = getServer();

    while((tmp = listIterate(server->clients, tmp)) != NULL) {
        client_t *client = tmp->value;

        if (client->fd != -1 && client->fd == fd){
            return client;
        }
    }

    logger->war("Faild To Find Client With FD: %d", fd);
    return NULL;
}

void kill_client(int fd) {
    server_t* server = getServer();
    int id = find_client_by_fd(fd);

    if (id != -1)
        deleteNode(server->clients, id);
}

void broadcast(char *msg, client_t* ignore) {
    Node* tmp = NULL;
    server_t* server = getServer();
    int nc = server->clients->nodeCount;

    int length = strlen(msg) + 2;
    char sendMsg[length];

    memset(sendMsg, 0, length);
    snprintf(sendMsg, length, " %s", msg);
    
    sendMsg[0] = length;

    logger->dbg("Broadcast msg (%d): %s for %d clients", length, sendMsg, nc);
  
    while((tmp = listIterate(server->clients, tmp)) != NULL) {
        if (tmp->value == ignore) {
            continue;
        }

        client_t *client = tmp->value;
        logger->dbg("Broadcast msg: %s", sendMsg);
        logger->dbg("msg Length: %d", length + 2);


        int i = send(client->fd, sendMsg, length + 2, 0);
        
        if (i != length + 2) {
          logger->err("Broadcast error for fd : %d / %d", i, length + 2);
        }
        else {
          logger->dbg("Broadcast success for fd : %d", client->fd);
        }
    }
}


void handleCommand(Player* p, char* msg) {
    logger->inf("Handeling Command: %s", msg);


    char* resp[3];
    explode(':', msg, 0, 0, resp);

    char* cmd = resp[0];
    int val = str2int(resp[1]);

    logger->dbg("CMD: %s Value: %d", cmd, val);

    logger->dbg("Calling Command");
    server_t* server = getServer();

    logger->dbg("Existing Commands: %d", server->commands->nodeCount);
    Node* n = getNodeByName(server->commands, cmd);

    if(n != NULL) {
        if(p->alive) {
            Arg* arg = n->value;
            arg->function(p, val);

        }
    }
    else{
        logger->dbg("Fail To Find Command: %s", cmd);
    }

    free(resp[0]);
    free(resp[1]);
}

void handle_client_sockets(int *client_socket, fd_set *readfds, struct sockaddr_in server) {
    //logger->inf("==== Handeling Clients ====");
    int sd;
    ssize_t valread;
    int addrlen = sizeof(&server);
    char buffer[MSG_SIZE];

    int i;
    Node* n = NULL;
    Game* game = getGame();
    //logger->err("-- getting Server");
    server_t* serv = getServer();

    while((n = listIterate(serv->clients, n)) != NULL) {
        //logger->err("-- Node: #%d => %d", n->id, n->name);

        client_t *client = n->value;
        int fd = client->fd;


        //logger->err("-- Checking fd: %d", fd);
        if (!FD_ISSET( fd , readfds)) {
            continue;
        }

        logger->err("-- Getting Message");
        if ((valread = recv( fd , buffer, MSG_SIZE, 0)) == 0) {
            logger->war("Client Disconected: %s @%p!!!", client->name, client->player);

            if(client->player != NULL) {
                char playerLeft[43];
                memset(playerLeft, 0, 43);
                snprintf(playerLeft, 43, "playerLeft:%d", client->player->id);
                
                if(client->player->alive && game->status <= GAME_RUNNING) {
                    killPlayer(client->player);
                }

                close(client->fd);
                free(client->name);
                
                n = n->prev;
                deleteNode(serv->clients, client->id);
                
                broadcast(playerLeft, getClient());
            }
            else{
                logger->war("CLIENT HAS NO PLAYER !!!");
            }
        }
        else {
            logger->err("-- Receiving Message");

            logger->err("Received Command: %s", buffer);
            handleCommand(client->player, buffer);
            //buffer[valread] = '\0';
            //read_command(buffer, fd, client_socket);
        }
    }
}

int network_handling (int master_socket_fd, struct sockaddr_in server) {
    static int *client_socket = NULL;

    fd_set readfds;
    int max_sd = 0;
    int activity = 0;

    Game* game = getGame();
    logger->enabled = game->flags & DBG_SERVER;

    if(game->status > GAME_END) {
        return 0;
    }

    logger->inf("==== HANDLE NETWORK ====");

    if(client_socket == NULL) {
        logger->inf("==== INIT CLIENTS SOCKETS ====");

        client_socket =  malloc(sizeof(int));

        int a = 0;
        for (a = 0; a < 4; a++) {
            client_socket[a] = 0;
        }

        logger->dbg("==== INIT CLIENTS SOCKETS DONE ====");
    }


    logger->dbg("-- Reset fd");
    //clear the socket set
    FD_ZERO(&readfds);

    //add master socket to set
    logger->dbg("-- Set Master fd");
    logger->dbg("-- test");

    logger->dbg("-- test %d", master_socket_fd);
    FD_SET(master_socket_fd, &readfds);
    logger->dbg("-- test DONE");

    logger->dbg("-- Update Max Sd: %d", master_socket_fd);
    max_sd = master_socket_fd;

    logger->dbg("-- Add Clients");
    max_sd = add_child_socket_to_set(max_sd, client_socket, &readfds);

    logger->dbg("-- Fetching Activity");

    struct timeval tv = {0, 15};
    activity = select( max_sd + 1 , &readfds , NULL , NULL , &tv);

    if ((activity < 0) && (errno!=EINTR)) {
        
        logger->dbg("-- Handle no Activity");
        return 0;
    }

    // logger->err("-- Handeler: Ask-Lock");
    lock(DBG_SERVER);
    // logger->err("-- Handeler: Lock");
    
    logger->dbg("-- Handeling Master Socket");
    handle_master_socket(master_socket_fd,  client_socket, server, &readfds);

    logger->dbg("-- Handeling Clients Socket");
    handle_client_sockets(client_socket, &readfds, server);
    // logger->err("-- Handeler: Un-Lock");
    unlock(DBG_SERVER);

    logger->dbg("==== HANDLE NETWORK DONE ====");
    return 1;
}
