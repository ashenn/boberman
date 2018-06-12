#include "../../libs/libmy_extended.h"
#include "../server.h"

void c_message_channel(char *buffer, int sd, int *client_socket) {
    my_putstr(buffer);

    int packet_length = 1 + my_strlen(buffer);
    char *packet = malloc(packet_length);
    packet[0] = 0x02;
    my_strcat(packet, buffer);

    if (broadcast(client_socket, my_getnbr(opts->next->next->value), sd, packet)==0)
        my_putstr("Failure Broadcasting Message :(\n");

    return;
}