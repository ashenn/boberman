#include "../../libs/libmy_extended.h"
#include "../server.h"

void init_commands () {
    command_t *login = (command_t*)malloc(sizeof(command_t));
    login->function = &(c_login);
    login->id = 0x01;

    command_t *message_channel = (command_t*)malloc(sizeof(command_t));
    message_channel->function = &(c_message_channel);
    message_channel->id = 0x02;

    command_t *message_private = (command_t*)malloc(sizeof(command_t));
    message_private->function = &(c_message_channel);
    message_private->id = 0x03;

    command_t *create_channel = (command_t*)malloc(sizeof(command_t));
    create_channel->function = &(c_create_channel);
    create_channel->id = 0x04;

    command_t *fetch_clients = (command_t*)malloc(sizeof(command_t));
    fetch_clients->function = &(c_fetch_clients);
    fetch_clients->id = 0x05;

    command_t *fetch_client = (command_t*)malloc(sizeof(command_t));
    fetch_client->function = &(c_fetch_client);
    fetch_client->id = 0x06;

    commands = create_list(login);
    add_node(&commands, message_channel);
    add_node(&commands, message_private);
    add_node(&commands, create_channel);
    add_node(&commands, fetch_clients);
    add_node(&commands, fetch_client);
}