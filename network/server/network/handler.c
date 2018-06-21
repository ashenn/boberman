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

    logger->war("New connection !!");
    if(serv->clients->nodeCount >= 4) {
        char motd[6];
        snprintf(motd, 6, "Fail:0");
        logger->dbg("Send Message: %s", motd);


        if( send(new_socket, motd, strlen(motd), 0) != strlen(motd) ) {
            logger->err("Faild To Send Message");
        }

        return;
    }

    
    logger->war("Adding New Player: %d", serv->clients->nodeCount+1);

    char clientName[12];
    snprintf(clientName, 12, "PLAYER-%d", serv->clients->nodeCount+1);
    logger->war("-- Name: %s", clientName);

    client_t* client = malloc(sizeof(client_t));
    
    Node* cliNode = addNodeV(serv->clients, clientName, client, 1);

    client->id = cliNode->id;

    logger->war("-- id: %d", client->id);

    char motd[43];
    snprintf(motd, 43, "ok:%d", client->id);
    logger->dbg("Send Message: %s", motd);

    if( send(new_socket, motd, strlen(motd), 0) != strlen(motd) ) {
        logger->err("Faild To Send Welcome Message");
    }

    for (int i = 0; i < 4; i++) {
        if( *(client_socket + i) == 0 ) {
            *(client_socket + i) = new_socket;
            logger->inf("Adding to list of sockets as %d\n" , i);
            break;
        }
    }

    //logger->war("HANDLER: Ask-Lock");
    lock(DBG_SERVER);
    //logger->war("HANDLER: Lock");
    
    client->fd = new_socket;
    client->name = clientName;
    client->player = genPlayer(clientName);
    if(serv->clients->nodeCount == 1) {
        initPlayer(client->player);
    }

    //logger->war("HANDLER: Un-Lock");
    unlock(DBG_SERVER);

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

void handleCommand(Player* p, char* msg) {
    logger->err("Handeling Command: %s", msg);


    char* resp[3];
    explode(':', msg, 0, 0, resp);

    char* cmd = resp[0];
    int val = str2int(resp[1]);

    logger->err("CMD: %s Value: %d", cmd, val);

    logger->err("Calling Command");
    server_t* server = getServer();

    logger->err("Existing Commands: %d", server->commands->nodeCount);
    Node* n = getNodeByName(server->commands, cmd);
    
    if(n != NULL) {
        Arg* arg = n->value;
        arg->function(p, val);
    }
    else{
        logger->err("Fail To Find Command: %s", cmd);
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
    //logger->err("-- getting Server");
    server_t* serv = getServer();

    while((n = listIterate(serv->clients, n)) != NULL) {
        logger->err("-- Node: #%d => %d", n->id, n->name);

        client_t *client = n->value;
        int fd = client->fd;
        
        
        logger->err("-- Checking fd: %d", fd);
        if (FD_ISSET( fd , readfds)) {
            
            logger->err("-- Getting Message");
            if ((valread = recv( fd , buffer, MSG_SIZE, 0)) == 0) {
                logger->err("-- Faild Deleting User");
                //getpeername(fd , (struct sockaddr*)&server , (socklen_t*)&addrlen);
                //Node *n = search_node_by_index(clients, find_client_by_fd(fd)).next;

                logger->war("Client Disconected: %s !!!", client->name);
                
                close( fd );
                *(client_socket + i) = 0;
                
                killPlayer(client->player);
            } else {
                logger->err("-- Receiving Message");

                logger->err("Received Command: %s", buffer);
                handleCommand(client->player, buffer);
                //buffer[valread] = '\0';
                //read_command(buffer, fd, client_socket);
            }
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

    if(game->status >= GAME_END) {
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
    struct timeval tv = {1, 0};
    activity = select( max_sd + 1 , &readfds , NULL , NULL , &tv);

    if ((activity < 0) && (errno!=EINTR))
    {
        logger->war("Faild To Get Activity From Socket !!!");
        logger->dbg("==== HANDLE NETWORK FAILD ====");
        return 0;
    }

    logger->dbg("-- Handeling Master Socket");
    handle_master_socket(master_socket_fd,  client_socket, server, &readfds);

    logger->dbg("-- Handeling Clients Socket");
    handle_client_sockets(client_socket, &readfds, server);

    logger->dbg("==== HANDLE NETWORK DONE ====");
    return 1;
}