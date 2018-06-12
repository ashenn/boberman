#include <errno.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/time.h>

typedef struct {
    char id;
    void (*function)(char* buffer, int sd);
} command_t;


void read_command(char *buffer, int sd);
typedef struct {
    int id;
    char *name;
} client_t;

extern List user_commands;
extern List commands;
extern List clients;

//commands
void init_commands();

char 	*readline(void);
void	quit(char* buffer, int sd);
void	joinChannel(char* buffer, int sd);
void    fetchClients(char* buffer, int sd);

char 	**init_commands_list(void);
void    c_recv_list_users(char *buffer, int sd);
void    c_recv_msg(char *buffer, int sd);
void c_recv_user(char *buffer, int sd);