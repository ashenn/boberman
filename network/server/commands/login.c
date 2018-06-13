#include "../../libs/libmy_extended.h"
#include "../server.h"

int new_client_id() {
    Node *tmp = clients;
    if (tmp->next != NULL)
        tmp = tmp->next;
    int ids[255];
    int id = -1;
    for (int i = 1; i < 255; i++)
        ids[i] = -1;
    while (tmp->next != NULL) {
        client_t *c = (client_t*)tmp->value;
        my_put_nbr(c->id);
        ids[c->id] = c->id;
        tmp = tmp->next;
    }
    for (int i = 1; i < 255; i++) {
        if (ids[i] == -1){
            id = i;
            break;
        }
    }
    return id;
}

void add_client(char *pseudo, int sd) {
    int id = new_client_id();

    client_t *new_client = malloc(sizeof(client_t));
    new_client->id = id;
    new_client->name = pseudo;
    //new_client->addr = addr;
    new_client->fd = sd;
    add_node(&clients, new_client);

    //my_putstr("infos : ");
    //my_put_nbr(ntohs(new_client->addr.sin_port));
}

void c_login(char *buffer, int sd, int *client_socket) {

    my_putstr("Login ... \nClients : ");
    my_put_nbr(clients->length);
    my_putstr("/");
    my_put_nbr(my_getnbr(opts->next->next->value));

    if (clients->length - 1 < my_getnbr(opts->next->next->value)) {
        add_client(buffer, sd);

        char *message = malloc(21 + my_strlen(buffer));
        sprintf(message, "%c%s a rejoint le chat.\n", 0x01, buffer);

        broadcast(client_socket, my_getnbr(opts->next->next->value), sd, message);
    }

    else
        printf("nope, plus de place");
}