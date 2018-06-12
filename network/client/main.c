#include "../libs/libmy_extended.h"
#include "client.h"

List user_commands = NULL;
List commands = NULL;
List clients = NULL;

int send_message_to_channel(int sockfd, char *msg) {
    int packet_length = 2 + my_strlen(msg);
    char *packet = malloc(packet_length);
    packet[0] = 0x02;
    packet[1] = 0x00;
    my_strcat(packet, msg);

    if ((send(sockfd, packet, strlen(packet), 0)) == -1) {
        my_putstr("Failure Sending Message\n");
        close(sockfd);
        return 0;
    }
    return 1;
}

int create_channel(int sockfd, char *msg) {
    int packet_length = 1 + my_strlen(msg);
    char *packet = malloc(packet_length);
    packet[0] = 0x04;
    my_strcat(packet, msg);

    if ((send(sockfd, packet, strlen(packet), 0)) == -1) {
        my_putstr("Failure Sending Message\n");
        close(sockfd);
        return 0;
    }
    return 1;
}

int send_login_request(int sockfd, char *name) {
    int packet_length = 1 + my_strlen(name);
    char *packet = malloc(packet_length);
    packet[0] = 0x01;
    packet[1] = 0x00;
    my_strcat(packet, name);

    if ((send(sockfd, packet, packet_length, 0)) == -1) {
        my_putstr("Failure Sending Message\n");
        close(sockfd);
        return 0;
    }
    return 1;
}

int c_fetch_client (int sockfd, int clientid) {
    char *packet = malloc(2);
    packet[0] = 0x6;
    packet[1] = clientid;

    if ((send(sockfd, packet, my_strlen(packet), 0)) == -1) {
        my_putstr("Failure Fetching client\n");
        close(sockfd);
        return 0;
    }
    return 1;
}

void c_recv_msg(char *buffer, int sd) {
    char *output = malloc (my_strlen(buffer) + 1);
    sprintf(output, "%s\n", buffer);
    my_putstr(output);
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

void c_recv_list_users(char *buffer, int sd) {
    for (int i = 0; i < my_strlen(buffer); i++) {
        my_putstr("hoy\n");
        int client_index = find_client_by_id(buffer[i]);
        if (client_index == -1) {
            my_putstr("hey\n");
            c_fetch_client(sd, buffer[i]);
        }
    }

    Node *tmp = clients;
    while (tmp != NULL) {
        client_t *client = tmp->value;
        my_putstr(client->name);
        tmp = tmp->next;
    }

}

void c_recv_user(char *buffer, int sd) {
    my_putstr("Meeting client");
    int client_index = find_client_by_id(buffer[0] + 1);
    int size = my_strlen(buffer);
    if (client_index == -1) {
        my_putstr("creating user\n");
        client_t *client = malloc(sizeof(client_t));
        char *name = malloc(size - 1);
        sprintf(name, "%*s", size - 1, buffer + 1);
        client->id = buffer[0];
        client->name = name;
        add_node(&clients, client);
    }

    Node *tmp = clients;
    while (tmp != NULL) {
        client_t *client = tmp->value;
        my_putstr(client->name);
        tmp = tmp->next;
    }

}

int main(int argc, char *argv[]) {
    // Init command and get command_list
    init_commands ();
    char **commands_list = init_commands_list ();


    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in client;
    Options opts = init_parser("null");
    List args = get_args(argc, argv);
    fd_set readfds;

    add_option(&opts, "-ip", 1, "localhost", my_putstr);
    add_option(&opts, "-p", 1, "8888", my_putstr);
    int i = parser(&opts, args);
    if (i == 1)
        return 0;

    client.sin_family = AF_INET;
    client.sin_addr.s_addr = inet_addr("localhost");
    client.sin_port = 0;

    bind(sockfd, (struct sockaddr *) &client, sizeof(client));

    struct sockaddr_in remoteaddr;
    remoteaddr.sin_family = AF_INET;
    remoteaddr.sin_addr.s_addr = inet_addr(opts->next->value);
    remoteaddr.sin_port = htons(my_getnbr(opts->next->next->value));

    if (connect(sockfd, (struct sockaddr *) &remoteaddr, sizeof(remoteaddr)) == -1) {
        my_putstr("Connection Failed\n");
        return 0;

    }

    my_putstr("Choisissez un pseudo : ");
    char *name = readline();
    int max_sd;
    send_login_request(sockfd, name);

    while (42) {
        char *server_reply = malloc(32);
        for (int i = 0; i < 32; i++)
            server_reply[i] = '\0';
        struct timeval tv;
        FD_ZERO(&readfds);
        tv.tv_sec = 3;
        tv.tv_usec = 50;

        //add master socket to set
        FD_SET(0, &readfds);
        FD_SET(sockfd, &readfds);
        max_sd = sockfd;
       // if (STDIN_FILENO > max_sd)
        //    max_sd = STDIN_FILENO;

        int activity = select(max_sd + 1, &readfds, NULL, NULL, &tv);

        if ((activity < 0) && (errno != EINTR)) {
            printf("select error");
        }

        char msg[30];
        int wasdone = 1;

        if (FD_ISSET(0, &readfds)) {
            wasdone = 0;

            for (int i = 0; i < 30; i++)
                msg[i] = 0x01;
            //readline();
            int i = read(0, msg, my_strlen(msg) - 1);
            if (i <= 0 )
                break;
            wasdone = 1;
            for (int i = 0; i < 30; i++)
                if (msg[i] == 0x01) {
                    msg[i - 1] = '\0';
                    break;
                }
                if (msg[0] == '/') {
                    my_putstr("debug1");
                    int s = 1;

                    while  (msg[s] != ' ' && msg[s] != '\0') {
                        ++s;
                    }
                    --s;
                    my_putstr("debug2");
                    char *command = malloc(s);
                    for (int i = 0; i < s; i++) {
                        command[i] = msg[i+1];
                    }
                    int command_id = -1;



                    for (int i = 0; i < my_strlen(*commands_list) - 1; i++){
                        my_putstr(command);
                        my_putstr( " vs ");
                        my_putstr(commands_list[i]);
                        my_putstr("\n");
                        if (my_strcmp(command, commands_list[i]) == 0) {
                            my_putstr("found : ");
                            my_put_nbr(i);
                            my_putstr(";");
                            command_id = i;
                            break;
                        }
                    }

                    if (command_id == -1) {
                        my_putstr("not found :(");
                        continue;
                    }

                    my_putstr("for command id : ");
                    my_put_nbr(command_id);
                    Node *command_node = search_node_by_index(user_commands, command_id).next;
                    my_putstr("b");
                    command_t *co = command_node->value;
                    my_putstr("c");
                    my_put_nbr(co->id);
                    co->function(msg, sockfd);
                    my_putstr("d");

                }
                else {
                send_message_to_channel(sockfd, msg);
                free(server_reply);
            }


        } else if (FD_ISSET(sockfd, &readfds)) {
            if (recv(sockfd, server_reply, 140, 0) < 0) {
                free(server_reply);
                my_putstr("Failure Recieve Message\n");
            } else {
                read_command(server_reply, sockfd);


            }
        }

    }
}
