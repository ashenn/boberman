#ifndef _GAME_H__
#define _GAME_H__

#include "../common.h"

#define GAME_MENU 0
#define GAME_LOBY 1
#define GAME_READY 2
#define GAME_SART 3
#define GAME_RUNNING 4
#define GAME_END 5
#define GAME_QUIT 6

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

typedef struct Game Game;
struct Game {
	short status;
	unsigned int flags;

	pthread_cond_t cond;
	pthread_mutex_t mutex;

	void* renderThread;
};

Game* getGame();
void quitGame();
Button** getMenu();
void mainMenu();
void changeGameStatus(short status);
void renderMap();
void* loadMap();
void parseGameArgs(int argc, char* argv[]);

void tick();

#endif