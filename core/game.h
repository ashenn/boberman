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

typedef struct Game Game;
struct Game {
	short status;
	unsigned int flags;
};

Game* getGame();
void quitGame();
Button** getMenu();
void mainMenu();
void changeGameStatus(short status);
void renderMap();
void* loadMap();
void parseGameArgs(int argc, char* argv[]);

#endif