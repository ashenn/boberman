#ifndef CLIENT_H
#define CLIENT_H

#include <errno.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/time.h>

#include "../../base/basic.h"
#include "../../base/libList.h"
#include "../../base/libParse.h"
#include "../../base/logger.h"

#include "../../main.h"

pthread_t clientThread;

typedef struct {
    int id;
    int fd;

    short init;

    char *name;
    struct sockaddr_in client;
    struct sockaddr_in server;
} Connexion;

short findHost();
void* clientProcess();
Connexion* getConnexion();

#endif // CLIENT_H