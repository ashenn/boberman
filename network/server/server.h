#ifndef HEADER_H
#define HEADER_H


#include "../../base/basic.h"
#include "../../base/libList.h"
#include "../../base/libParse.h"
#include "../../base/logger.h"

#include "../../main.h"
#include "../../common.h"


#include <unistd.h>
#include <errno.h>
#include <pthread.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/time.h>

typedef struct {
    struct sockaddr_in addr;
    int fd;
    int id;

    char* name;
    Player* player;
} client_t;

typedef struct {
    int fd;
    int port;
    struct sockaddr_in addr;

    ListManager* clients;
    ListManager* commands;
} server_t;

typedef struct {
    char id;
    void (*function)(char* buffer, int sd, int *client_socket);
} command_t;

typedef struct {
    int id;
    char *name;
    ListManager* clients;
} channel_t;

void* serverProcess();
server_t* getServer();
server_t* initServer(short init);

#include "../../core/game.h"

void broadcast(char *msg, client_t* ignore);
client_t* getClient();


//network
void handle_client_sockets(int *client_socket, fd_set *readfds, struct sockaddr_in server);
int network_handling (int socket_fd, struct sockaddr_in server);
server_t* socket_init(unsigned int addr, int port);
void handle_master_socket(int master_socket_fd, int *client_socket, struct sockaddr_in server, fd_set *readfds);
//int broadcast(int *clients, int maxclient, int self, char *buffer);

//protocol
void read_command(char *buffer, int sd, int* client_socket);

//commands
void init_commands();
void c_login(char *buffer, int sd, int *client_socket);
void add_client(char *pseudo, int sd);
int find_client_by_id(int id);
int find_client_by_fd(int fd);
int send_to_client(int client, char *buffer);
void c_fetch_clients(char *buffer, int sd, int *client_socket);
void c_fetch_client(char *buffer, int sd, int *client_socket);
//void c_message_channel(char *buffer, int sd, int *client_socket);
void c_create_channel(char *buffer, int sd, int *client_socket);

#endif
