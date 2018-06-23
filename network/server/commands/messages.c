#include "../server.h"

/*void c_message_channel(char *buffer, int sd, int *client_socket) {
    logger->inf("==== Received Mesage ====");
    logger->dbg("-- from sd: #%d", client_socket);
    logger->dbg("%s", buffer);

    int packet_length = 1 + strlen(buffer);
    char *packet = malloc(packet_length);
    packet[0] = 0x02;
    strcat(packet, buffer);

    Game* game = getGame();
    if (broadcast(client_socket, 4, sd, packet)==0) {
    	logger->err("Failure Broadcasting Message !!!!");
    }

    return;
}*/
