#ifndef _MAP_H__
#define _MAP_H__

#include "../common.h"

typedef struct Block Block;
struct Block {
	int id;
	char* name;
	short state;
	short destroyed;
	int bonusType;

	Object* obj;
	SDL_Rect clip;
};

void generateWalls();
void generateBlocks();

void breakBlock(Block* block);
ListManager* getBlockList();

#endif
