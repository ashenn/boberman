#include "../../libs/libmy_extended.h"
#include "../server.h"

int new_channel_id() {
    Node *tmp = channels;
    if (tmp->next != NULL)
        tmp = tmp->next;
    int ids[255];
    int id = 0;
    for (int i = 1; i < 255; i++)
        ids[i] = 0;
    while (tmp->next != NULL) {
        channel_t *c = (channel_t*)tmp->value;
        ids[c->id] = c->id;
        tmp = tmp->next;
    }
    for (int i = 1; i < 255; i++) {
        if (ids[i] == 0){
            id = i;
            break;
        }
    }
    return id;
}

void c_create_channel_succes(char *name, int id, int *client_socket) {
    int packet_length = 2 + my_strlen(name);
    char *packet = malloc(packet_length);
    packet[0] = 0x05;
    packet[1] = (char)id;
    my_strcat(packet, name);

    if (broadcast(client_socket, my_getnbr(opts->next->next->value), -1, packet)==0)
        my_putstr("Failure Broadcasting Message :(\n");

    return;
}

void c_create_channel(char *buffer, int sd, int *client_socket) {
    my_putstr("new channel");
    channel_t *new_channel = malloc(sizeof(channel_t));
    new_channel->name = buffer;
    client_t *creator = (client_t*)search_node_by_index(clients, find_client_by_fd(sd)).value;
    new_channel->clients = create_list(creator);
    new_channel->id = new_channel_id();
    my_put_nbr(client_socket[0]);

    my_putstr(" {");
    my_putstr(buffer);
    my_putstr(", ");
    my_put_nbr(new_channel->id);
    my_putstr("}");

    add_node(&channels, new_channel);

    c_create_channel_succes(buffer, new_channel->id, client_socket);
    return;
}

