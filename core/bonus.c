#include <time.h>
#include "bonus.h"

void incSpeed(Object* bnsObj, Object* playerObj) {

	if (playerObj->containerType == PLAYER) {
		//logger->err("TEST INCREES SPEED");
	}
}

void incBomb(Object* bnsObj, Object* playerObj) {
	logger->err("TEST INCREES BOM: %s", playerObj->name);
	
	if (playerObj->containerType == PLAYER) {

		Player* p = (Player*) playerObj->container;
		if (p->bombPower < 4) {
			p->bombPower ++;
		}

		bnsObj->collision->enabled = 0;
	}
}

void* getBonusFunc(short type) {
	switch	(type) {
		case BNS_SPEED:
			return &incSpeed;
			break;

		case BNS_BOMB:
			return &incBomb;
			break;
	}

	return NULL;
}

void generateBonus(SDL_Rect pos) {
	static short init  = 0;
	if (!init) {
		srand(time(NULL));
		init = 1;
	}

	Bonus* bns = malloc(sizeof(Bonus));
	bns->z = 1;

	int type = rand() % (1 - 0) + 0;
	logger->err("TEST BONUS: %d", type);
	switch	(type) {
		case BNS_SPEED:
			logger->err("ADD SPEED");
			bns->clip.y = 0;
			bns->clip.x = 4 * CELL_SIZE;

			bns->clip.w = CELL_SIZE;
			bns->clip.h = CELL_SIZE;
			break;

		case BNS_BOMB:
			logger->err("ADD BBOMB");
			bns->clip.y = 0;
			bns->clip.x = 2 * CELL_SIZE;

			bns->clip.w = CELL_SIZE;
			bns->clip.h = CELL_SIZE;
			break;
	}

	AssetMgr* ast = getAssets();
	SDL_Surface* img = ast->getImg("objects");
	bns->obj = addSimpleObject("Bonus", img, NULL, 1);

	bns->obj->pos.x = pos.x;
	bns->obj->pos.y = pos.y;

	SDL_Rect hit = {0, 0, CELL_SIZE, CELL_SIZE};
	setHitBox(bns->obj, hit, 0, 1);

	bns->obj->container = bns;
	bns->obj->containerType = BONUS;
	bns->obj->collision->fnc = getBonusFunc(type);
	bns->obj->collision->deleteOnCol = 1;
	bns->obj->collision->flag = COL_BONUS;
	bns->obj->collision->colFlags = COL_PLAYER;// | COL_BOMB;
	
	bns->obj->clip = &bns->clip;
}