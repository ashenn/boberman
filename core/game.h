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

typedef struct Game Game;
struct Game {
	short status;
};

Game* getGame();
void quitGame();
Button** getMenu();
void mainMenu();
void changeGameStatus(short status);
void renderMap();
void* loadMap();

#endif