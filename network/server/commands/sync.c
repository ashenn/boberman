#include "../server.h"


void c_fetch_clients(char *buffer, int sd, int *client_socket) {
    my_putstr("STARTING===\n");
    (void) buffer;
    (void) sd;
    (void) client_socket;
    char *packet = malloc(clients->length);
    packet[0] = 0x04;
    Node *tmp = clients;
    int i = 1;
    while (tmp != NULL) {
        if (tmp->prev == NULL) {
            tmp = tmp->next;
            continue;
        }
        client_t *theclient = tmp->value;
        packet[i] = theclient->id;
        my_put_nbr(packet[i]);
        i++;
        tmp = tmp->next;
    }
    packet[i] = 0x00;
    (void) buffer;
    (void) client_socket;

    if (send(sd, packet, my_strlen(packet), 0) == 0)
        my_putstr("ccccc");

}

void c_fetch_client(char *buffer, int sd, int *client_socket) {
    (void) client_socket[0];
    (void) sd;

    my_putstr("bb");
    my_put_nbr(buffer[0]);
    int client_index = find_client_by_id(buffer[0]);
    my_putstr("ind=");
    my_put_nbr(client_index);
    if (client_index != -1) {
        Node client_node = search_node_by_index(clients, client_index + 1);
        client_t *client = client_node.value;
        my_put_nbr(client->id);
        char *packet = malloc(my_strlen(client->name) + 2);
        my_putstr("dd");
        sprintf(packet, "%c%c%s", 0x05, (char) client->id, client->name);
        my_putstr("ee");
        send_to_client(sd, packet);
        my_putstr("ff");
        return;
    }


}