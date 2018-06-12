#ifndef HEADER_H
#define HEADER_H

#include <errno.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/time.h>

typedef struct {
    struct sockaddr_in addr;
    int fd;
    int id;
    char *name;
} client_t;

typedef struct {
    struct sockaddr_in addr;
    int fd;
} server_t;

typedef struct {
    char id;
    void (*function)(char* buffer, int sd, int *client_socket);
} command_t;

typedef struct {
    int id;
    char *name;
    List clients;
} channel_t;

extern List commands;
extern List clients;
extern List channels;
extern Options opts;
extern Logger logger;

//network
void handle_client_sockets(int *client_socket, fd_set *readfds, struct sockaddr_in server);
int network_handling (int socket_fd, struct sockaddr_in server);
server_t socket_init(unsigned int addr, int port);
void handle_master_socket(int master_socket_fd, int *client_socket, struct sockaddr_in server, fd_set *readfds);
int broadcast(int *clients, int maxclient, int self, char *buffer);

//protocol
void read_command(char *buffer, int sd, int* client_socket);

//commands
void init_commands ();
void c_login(char *buffer, int sd, int *client_socket);
void add_client(char *pseudo, int sd);
int find_client_by_id(int id);
int find_client_by_fd(int fd);
int send_to_client(int client, char *buffer);
void c_fetch_clients(char *buffer, int sd, int *client_socket);
void c_fetch_client(char *buffer, int sd, int *client_socket);
void c_message_channel(char *buffer, int sd, int *client_socket);
void c_create_channel(char *buffer, int sd, int *client_socket);

#endif