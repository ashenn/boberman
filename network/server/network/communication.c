#include "../../libs/libmy_extended.h"
#include "../server.h"

int broadcast(int *clients, int maxclient, int self, char *buffer) {
    for (int i = 0; i < maxclient; ++i) {
        if (*(clients + i) != self)
            if (send (*(clients + i), buffer, my_strlen(buffer), 0) == 0)
                return 0;
    }
    return 1;
}

int send_to_client(int client, char *buffer) {
    if (send (client, buffer, my_strlen(buffer), 0) == 0)
        return 0;
    return 1;
}