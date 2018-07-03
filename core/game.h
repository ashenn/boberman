#ifndef _GAME_H__
#define _GAME_H__

#include "../common.h"
#include "view.h"
#include "../network/server/server.h"
#include "../network/client/client.h"

#define DEFAULT_PORT 8888

#define GAME_MENU 0
#define GAME_LOBY 1
#define GAME_READY 2
#define GAME_START 3
#define GAME_RUNNING 4
#define GAME_END 5
#define GAME_TIMEOUT 6
#define GAME_QUIT 7

#define NO_FLAG 0
#define DBG_HIT 1
#define DBG_EVNT 2
#define DBG_MOVE 4
#define DBG_VIEW 8
#define DBG_BOMB 16
#define DBG_MOUSE 32
#define DBG_ASSET 64
#define DBG_MAP 128
#define DBG_OBJ 256
#define DBG_PLAYER 512
#define DBG_STATE 1024
#define DBG_MENU 2048
#define DBG_ANIM 4096
#define DBG_BONUS 8192
#define DBG_SERVER 16384
#define DBG_CLIENT 32768

typedef struct GameOptions GameOptions;
struct GameOptions {
	int port;
	char ip[16];
};

typedef struct Game Game;
struct Game {
	short status;
	short isServer;
	unsigned int flags;
	ListManager* flagList;

	pthread_cond_t cond;
	pthread_mutex_t mutex;

	pthread_t serverThread;
	pthread_t clientThread;
	pthread_t renderThread;

	GameOptions options;
};

Game* getGame();
void quitGame();
struct Button** getMenu();
void mainMenu();
void changeGameStatus(short status);
void renderMap();
void* loadMap();
void parseGameArgs(int argc, char* argv[]);

void tick();
void launchSate(short status);

void startGame();

void timer();

#endif