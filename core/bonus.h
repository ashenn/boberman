#ifndef __BONUS_H__
#define __BONUS_H__

#include "../common.h"

typedef enum BonusType BonusType;
enum BonusType {
	BNS_NONE,
	BNS_BOMB,
	MAL_BOMB,
	BNS_EXPL,
	MAL_EXPL,
	BNS_SPEED,
	MAL_SPEED,
	BNS_SHOOT,
	BNS_SHIELD,
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

void generateBonus(SDL_Rect pos, BonusType type);
BonusType prepareBonus();

ListManager* getBonusList();
Bonus* getBonusById(int id);
#endif
