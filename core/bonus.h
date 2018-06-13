#ifndef __BONUS_H__
#define __BONUS_H__

#include "../common.h"

typedef enum BonusType BonusType;
enum BonusType {
	BNS_BOMB,
	BNS_SPEED
};

typedef struct Bonus Bonus;
struct Bonus {
	int z;
	int id;
	char* name;

	Object* obj;
	SDL_Rect clip;
	BonusType type;
};

void generateBonus();

#endif