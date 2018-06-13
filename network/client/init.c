#include "../libs/libmy_extended.h"
#include "client.h"

void init_commands () {
    client_t *dummyclient = malloc(sizeof(client_t));
    dummyclient->id = -1;
    dummyclient->name = "BOT";
    clients = create_list(dummyclient);

    command_t *recv_login = malloc(sizeof(command_t));
    recv_login->id = 0x01;
    recv_login->function = &(c_recv_msg);

    command_t *recv_msg = malloc(sizeof(command_t));
    recv_msg->id = 0x02;
    recv_msg->function = &(c_recv_msg);

    command_t *recv_motd = malloc(sizeof(command_t));
    recv_motd->id = 0x03;
    recv_motd->function = &(c_recv_msg);

    command_t *fetch_clients = malloc(sizeof(command_t));
    fetch_clients->id = 0x04;
    fetch_clients->function = &(c_recv_list_users);

    command_t *dede = malloc(sizeof(command_t));
    dede->id = 0x04;
    dede->function = &(c_recv_user);

    command_t *fetch_client = malloc(sizeof(command_t));
    fetch_client->id = 0x05;
    fetch_client->function = &(c_recv_user);

    commands = create_list(recv_login);
    add_node(&commands, recv_msg);
    add_node(&commands, recv_motd);
    add_node(&commands, fetch_clients);
    add_node(&commands, dede);
    add_node(&commands, recv_msg);


    command_t *exit = malloc(sizeof(command_t));
    exit->function = &(quit);

    command_t *join = malloc(sizeof(command_t));
    join->function = &(joinChannel);

    command_t *dezdz = malloc(sizeof(command_t));
    dezdz->function = &(fetchClients);

    command_t *sync = malloc(sizeof(command_t));
    sync->function = &(joinChannel);

    user_commands = create_list(exit);
    add_node(&user_commands, join);
    add_node(&user_commands, dezdz);
    add_node(&user_commands, join);

}

char** init_commands_list() {
    char **commands_list;
    *commands_list = malloc(4);

    commands_list[0] = "exit";
    commands_list[1] = "join";
    commands_list[2] = "fetch";
    commands_list[3] = "list_channels";

    return commands_list;
}