#include "../../libs/libmy_extended.h"
#include "../server.h"

int add_child_socket_to_set(int max_sd, int *client_socket, fd_set *readfds) {
    int sd;
    for (int i = 0 ; i < my_getnbr(opts->next->next->value) ; i++)
    {
        sd = *(client_socket + i);

        if(sd > 0)
            FD_SET( sd , readfds);

        if(sd > max_sd)
            max_sd = sd;
    }
    return max_sd;
}

char *tamer (char *b) {
    char *c = malloc(my_strlen(b) + 1);
    c[0] = 0x03;
    for (int i =0; i < my_strlen(b); i++)
        c[i+1] = b[i];
    return c;
}


void handle_master_socket(int master_socket_fd, int *client_socket, struct sockaddr_in server, fd_set *readfds) {
    int new_socket;
    char *txt = malloc(42);
    txt = "Bienvenue sur mon serveur !\n";
    char *motd = tamer(txt);
    my_putstr(motd);
    int addrlen = sizeof(&server);

    if (FD_ISSET(master_socket_fd, readfds)) {
        if ((new_socket = accept(master_socket_fd, (struct sockaddr *)&server, (socklen_t*)&addrlen))<0) {
            perror("accept");
            exit(EXIT_FAILURE);
        }
        info(logger, "New connection !!\n");
        if( send(new_socket, motd, my_strlen(motd), 0) != my_strlen(motd) )
            perror("send");

        for (int i = 0; i < my_getnbr(opts->next->next->value); i++) {
            if( *(client_socket + i) == 0 ) {
                *(client_socket + i) = new_socket;
                printf("Adding to list of sockets as %d\n" , i);
                break;
            }
        }
    }
}
int find_client_by_fd(int fd) {
    my_putstr("all : {");
    Node *tmp = clients;
    int i = 0;
    while (tmp != NULL) {
        client_t *client = tmp->value;
        if (client->fd != -1) {
            if (client->fd == fd){
                my_putstr("}\n");
                return i;
            }
        }
        i++;
        tmp = tmp->next;
    }
    my_putstr("}\n");
    return -1;
}

int find_client_by_id(int id) {
    Node *tmp = clients;
    int i = 0;
    while (tmp != NULL) {
        client_t *client = tmp->value;
        if (client->id == id) {
            return i;
        }
        i++;
        tmp = tmp->next;
    }
    return -1;
}

void kill_client(int fd) {
    int i = find_client_by_fd(fd);
    if (i != -1)
        del_node(&clients, i);
}

void handle_client_sockets(int *client_socket, fd_set *readfds, struct sockaddr_in server) {
    int sd;
    ssize_t valread;
    int addrlen = sizeof(&server);
    char buffer[1025];

    for (int i = 0; i < my_getnbr(opts->next->next->value); i++) {
       sd = *(client_socket + i);

        if (FD_ISSET( sd , readfds)) {
            if ((valread = recv( sd , buffer, 1024, 0)) == 0) {
                getpeername(sd , (struct sockaddr*)&server , (socklen_t*)&addrlen);
                Node *n = search_node_by_index(clients, find_client_by_fd(sd)).next;
                client_t *theclient = n->value;
                char *disconect_notif = malloc (100);
                sprintf(disconect_notif, " %s has left the chat \n", theclient->name);
                broadcast(client_socket, my_getnbr(opts->next->next->value), -1, disconect_notif);
                close( sd );
                *(client_socket + i) = 0;
                kill_client(sd);
            } else {
                buffer[valread] = '\0';
                my_putstr("find val : ");
                read_command(buffer, sd, client_socket);
        }
        }
    }
}

int network_handling (int master_socket_fd, struct sockaddr_in server) {
    fd_set readfds;
    int max_sd, activity, i;
    int *client_socket = malloc(sizeof(int) * my_getnbr(opts->next->next->value));

    for (i = 0; i < my_getnbr(opts->next->next->value); i++)
        client_socket[i] = 0;

    while (1) {
        //clear the socket set
        FD_ZERO(&readfds);

        //add master socket to set
        FD_SET(master_socket_fd, &readfds);
        max_sd = master_socket_fd;

        max_sd = add_child_socket_to_set(max_sd, client_socket, &readfds);

        activity = select( max_sd + 1 , &readfds , NULL , NULL , NULL);

        if ((activity < 0) && (errno!=EINTR))
        {
            printf("select error");
        }

        handle_master_socket(master_socket_fd,  client_socket, server, &readfds);

        handle_client_sockets(client_socket, &readfds, server);
    }
}