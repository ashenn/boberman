#include <time.h>
#include "bonus.h"

void incSpeed(Object* bnsObj, Object* playerObj) {
	if (playerObj->containerType == PLAYER) {
		Player* p = (Player*) playerObj->container;
		if (p->speed < 2) {
			p->speed += 0.25f;

			enableLogger(DBG_BONUS);
			logger->dbg("Inc Speed %s: %d", p->name, p->speed);
		}

		bnsObj->collision->enabled = 0;
	}
}

void decSpeed(Object* bnsObj, Object* playerObj) {
	if (playerObj->containerType == PLAYER) {
		Player* p = (Player*) playerObj->container;
		if (p->speed > 0.5f) {
			p->speed -= 0.25f;
			enableLogger(DBG_BONUS);
			logger->dbg("Dec Speed %s: %d", p->name, p->speed);
		}

		bnsObj->collision->enabled = 0;
	}
}

void incExpl(Object* bnsObj, Object* playerObj) {
	if (playerObj->containerType == PLAYER) {

		Player* p = (Player*) playerObj->container;
		if (p->bombPower < 4) {
			enableLogger(DBG_BONUS);
			logger->dbg("Inc Explosion %s: %d", p->name, p->bombPower);
			p->bombPower ++;
		}

		bnsObj->collision->enabled = 0;
	}
}

void decExpl(Object* bnsObj, Object* playerObj) {
	if (playerObj->containerType == PLAYER) {

		Player* p = (Player*) playerObj->container;
		if (p->bombPower > 1) {
			enableLogger(DBG_BONUS);
			logger->dbg("Dec Explosion %s: %d", p->name, p->bombPower);
			p->bombPower--;
		}

		bnsObj->collision->enabled = 0;
	}
}

void incBomb(Object* bnsObj, Object* playerObj) {
	if (playerObj->containerType == PLAYER) {

		Player* p = (Player*) playerObj->container;
		if (p->bombMax < 3) {
			enableLogger(DBG_BONUS);

			p->bombCnt++;
			p->bombMax++;
			logger->dbg("Inc Bomb Count %s: %d", p->name, p->bombMax);
		}

		bnsObj->collision->enabled = 0;
	}
}

void decBomb(Object* bnsObj, Object* playerObj) {
	if (playerObj->containerType == PLAYER) {

		Player* p = (Player*) playerObj->container;
		if (p->bombMax > 1) {
			enableLogger(DBG_BONUS);

			p->bombCnt--;
			p->bombMax--;
			logger->dbg("Dec Bomb Count %s: %d", p->name, p->bombMax);
		}

		bnsObj->collision->enabled = 0;
	}
}

void addShoot(Object* bnsObj, Object* playerObj) {
	if (playerObj->containerType == PLAYER) {

		Player* p = (Player*) playerObj->container;
		p->shoot = 1;

		bnsObj->collision->enabled = 0;
	}
}

void* getBonusFunc(short type) {
	switch	(type) {
		case MAL_BOMB:
			return &decBomb;
			break;

		case BNS_BOMB:
			return &incBomb;
			break;

		case MAL_EXPL:
			return &decExpl;
			break;

		case BNS_EXPL:
			return &incExpl;
			break;

		case BNS_SPEED:
			return &incSpeed;
			break;

		case MAL_SPEED:
			return &decSpeed;
			break;

		case BNS_SHOOT:
			return &decSpeed;
			break;
	}

	return NULL;
}

BonusType prepareBonus() {
	static short init  = 0;

	enableLogger(DBG_BONUS);

	if (!init) {
		srand(time(NULL));
		init = 1;
	}

	int r = rand() % 2;
	short spawn = !r;
	if (!spawn) {
		return -1;
	}
	int type = rand() % 6;
	logger->dbg("=== GENERATE BONUS %d ===", type);
	return type;
}

void generateBonus(SDL_Rect pos, BonusType type) {
	Bonus* bns = malloc(sizeof(Bonus));
	bns->z = 1;
	bns->clip.y = 0;
	bns->clip.w = BONUS_SIZE;
	bns->clip.h = BONUS_SIZE;

	switch	(type) {
		case BNS_BOMB:
			logger->dbg("-- Add BNS BOMB");
			bns->clip.x = 2 * BONUS_SIZE;
			break;

		case MAL_BOMB:
			logger->dbg("-- Add MAL BOMB");
			bns->clip.x = 3 * BONUS_SIZE;
			break;

		case BNS_EXPL:
			logger->dbg("-- Add BNS EXPLOSION");
			bns->clip.x = 0;
			break;

		case MAL_EXPL:
			logger->dbg("-- Add MAL EXPLOSION");
			bns->clip.x = BONUS_SIZE;
			break;

		case BNS_SPEED:
			logger->dbg("-- Add MAL SPEED");
			bns->clip.x = 4 * BONUS_SIZE;
			break;

		case MAL_SPEED:
			logger->dbg("-- Add MAL SPEED");
			bns->clip.x = 5 * BONUS_SIZE;
			break;

		case BNS_SHOOT:
			logger->dbg("-- Add BNS SHOOT");
			bns->clip.x = 6 * BONUS_SIZE;
			break;
	}

	AssetMgr* ast = getAssets();
	SDL_Surface* img = ast->getImg("objects");
	bns->obj = addSimpleObject("Bonus", img, NULL, 1);

	bns->obj->pos.x = pos.x;
	bns->obj->pos.y = pos.y;

	SDL_Rect hit = {0, 0, BONUS_SIZE, BONUS_SIZE};
	setHitBox(bns->obj, hit, 0, 1);

	bns->obj->container = bns;
	bns->obj->containerType = BONUS;
	bns->obj->collision->fnc = getBonusFunc(type);
	bns->obj->collision->deleteOnCol = 1;

	bns->obj->collision->flag = COL_BONUS;
	bns->obj->collision->colFlags = COL_PLAYER;// | COL_BOMB;

	bns->obj->clip = &bns->clip;
	bns->obj->lifetime = 10;
}
