#include "../server.h"

void read_command(char *buffer, int sd, int *client_socket) {
    int command_id = (int)buffer[0];
    // my_putstr(" Packet Command ID : ");
    // my_put_nbr(command_id);
    // my_putstr("\n");

    int size = strlen(buffer);

    char *shifted_buffer = malloc(size - 1);
    sprintf(shifted_buffer, "%*s", size - 1, buffer + 1);

    //Node command_node =  search_node_by_index(commands, command_id);
    /*
    if (command_node.value == NULL){
        //my_putstr("Invalid packed recieved\n");
    }
    else {
        //my_putstr("Valid packed recieved\n");
        command_t *c = command_node.value;
        c->function(shifted_buffer, sd, client_socket);
    }
    */
}