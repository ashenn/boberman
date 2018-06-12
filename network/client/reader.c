#include "../libs/libmy_extended.h"
#include "client.h"

void read_command(char *buffer, int sd) {
    int command_id = (int)buffer[0];
    my_putstr(" Packet Command ID : ");
    my_put_nbr(command_id);
    my_putstr("\n");

    int size = my_strlen(buffer);

    char *shifted_buffer = malloc(size - 1);
    sprintf(shifted_buffer, "%*s", size - 1, buffer + 1);

    Node command_node =  search_node_by_index(commands, command_id);

    if (command_node.value != NULL){
        command_t *c = command_node.value;

        c->function(shifted_buffer, sd);
    }
    else {
        my_putstr("fuck");
    }
}