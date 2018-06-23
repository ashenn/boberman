#include "bomb.h"

void bombMove(AnimParam* anim) {
	Object* obj = (Object*) anim->obj;

	if (obj->container == NULL) {
		return;
	}

	Bomb* bomb = (Bomb*) obj->container;

	bomb->pos.x = obj->pos.x;
	bomb->pos.y = obj->pos.y;

	if (bomb->moveIterate++ > 5){
		return;
	}

	int x = obj->pos.x;
	int y = obj->pos.y;

	switch (bomb->direction) {
		case UP:
			y -= 10;
			break;

		case DOWN:
			y += 10;
			break;

		case LEFT:
			x -= 10;
			break;

		case RIGHT:
			x += 10;
			break;
	}

	anim = moveTo(obj, x, y, 0.2f, 0);
	if (anim == NULL) {
		return;
	}

	anim->callBack = bombMove;
}

void bombHit(Object* bombObj, Object* targetObj) {
	if (bombObj->container == NULL) {
		return;
	}

	logger->err("=== BOMB COL ===");

	logger->err("-- Cast To Bomb");
	Bomb* bomb = (Bomb*) bombObj->container;

	if (bomb->exploded  || bomb->state < 3) {
		logger->err("-- Cant Move: %d state", bomb->state);
		return;
	}

	if (targetObj->container == NULL || targetObj->containerType != PLAYER) {
		logger->err("-- Not Player");
		return;
	}

	logger->err("-- Cast To Player");
	Player* p = (Player*) targetObj->container;

	if (0 && !p->shoot) {
		logger->err("-- Can't Shoot");
		return;
	}

	logger->err("-- Calc Pos");
	int x = bombObj->pos.x;
	int y = bombObj->pos.y;

	switch (p->direction) {
		case UP:
			y -= 30;
			break;

		case DOWN:
			y += 30;
			break;

		case LEFT:
			x -= 30;
			break;

		case RIGHT:
			x += 30;
			break;
	}

	logger->err("-- Move To x: %d, y: %d", x, y);

	bombObj->collision->enabled = 0;
	AnimParam* anim = moveTo(bombObj, x, y, 0.2f, 0);

	if (anim == NULL) {
		logger->err("-- Cant't Move To x: %d, y: %d", x, y);
		return;
	}

	anim->callBack = bombMove;
}

void explosionHit(Object* explObj, Object* targetObj) {
	if (targetObj->container == NULL) {
		return;
	}

	Player *killedPlayer;
	Block* block;
	Game* game = getGame();
	char playerkilled[43];
	char blockBreaked[43];
	
	switch(targetObj->containerType) {
		case PLAYER:
			if(game->isServer) {
				killedPlayer = targetObj->container;
				memset(playerkilled, 0, 43);
				snprintf(playerkilled, 43, "playerkilled:%d", killedPlayer->id);
				broadcast(playerkilled);
			}

			killPlayer(targetObj->container);
			break;

		case BLOCK:
			logger->err("Breaking Block");
			
			if(game->isServer) {
				logger->err("Is Server");
				block = targetObj->container;
				if(block->destroyed) {
					return;
				}

				
				memset(blockBreaked, 0, 43);
				snprintf(blockBreaked, 43, "breackblock:%d", block->id);
				logger->err("Msg: %s", blockBreaked);
				broadcast(blockBreaked);

				logger->err("BroadCast DONE");
			}
			
			logger->err("Breaking");
			breakBlock(targetObj->container);
			break;
	}
}

void getExplPartName(ExplosionPart part, char* name) {
	switch (part) {
		case EXPL_CENTER:
			snprintf(name, 15, "EXPL_CENTER");
			break;
		case EXPL_MID_H:
			snprintf(name, 15, "EXPL_MID_H");
			break;
		case EXPL_MID_V:
			snprintf(name, 15, "EXPL_MID_V");
			break;
		case EXPL_END_TOP:
			snprintf(name, 15, "EXPL_END_TOP");
			break;
		case EXPL_END_DOWN:
			snprintf(name, 15, "EXPL_END_DOWN");
			break;
		case EXPL_END_LEFT:
			snprintf(name, 15, "EXPL_END_LEFT");
			break;
		case EXPL_END_RIGHT:
			snprintf(name, 15, "EXPL_END_RIGHT");
			break;
		default:
			snprintf(name, 15, "BAD EXPL_INDEX");
			break;
	}
}

void clearExplosion(Object* obj) {
	enableLogger(DBG_BOMB);

	logger->inf("==== CLEAR EXPLOSION ====");

	logger->dbg("-- Name %s", obj->name);
	Node* n = NULL;

	while((n = listIterate(obj->childs, n)) != NULL) {
	    Object* child = (Object*) n->value;
		logger->dbg("-- Clear Child %s", child->name);

	}

	logger->dbg("==== CLEAR EXPLOSION DONE ====", obj->name);
}

void iterateExplosion(AnimParam* anim) {
	enableLogger(DBG_BOMB);

	logger->inf("==== ITARATE EXPLOSION ====");
	if (anim->obj->container == NULL) {
		logger->dbg("Explosion Container Is Empty");
		return;
	}

	Bomb* bomb = (Bomb*) anim->obj->container;
	bomb->state++;

	int y =  bomb->state - 9;
	logger->dbg("-- Explosion State: %d", y);

	if (y > 3){
		anim->deleteObject = 1;
		logger->dbg("-- Delete Object");
	}
	else{
		logger->dbg("-- Adjust Clip");
		SDL_Rect animClip = {0, (y+1) * BOMB_SIZE, BOMB_SIZE, BOMB_SIZE};
		logger->dbg("-- x: %d, y: %d", 0, y);

		anim = spriteAnim(anim->obj, animClip, 0.1f, 0, 1);
		anim->callBack = iterateExplosion;
	}
}

Object* genExplosionPart(ExplosionPart index, int x, int y) {
	enableLogger(DBG_BOMB);
	logger->inf("==== GENERATING EXPLOSION PART ====");

	char partName[15];
	getExplPartName(index, partName);
	logger->dbg("-- partName %s", partName);


	AssetMgr* ast = getAssets();
	Object* part = genSimpleObject(partName, ast->getImg("bomb3"), NULL, 2);

	logger->dbg("-- pos: x: %d, y: %d", x, y);
	part->pos.x = BOMB_SIZE * x;
	part->pos.y = BOMB_SIZE * y;

	short clipX=0;
	part->clip = malloc(sizeof(SDL_Rect));
	switch (index) {
		case EXPL_MID_H:
			clipX = 1;
			break;

		case EXPL_MID_V:
			clipX = 5;
			break;

		case EXPL_END_TOP:
			clipX = 6;
			break;

		case EXPL_END_DOWN:
			clipX = 8;
			break;

		case EXPL_END_LEFT:
			clipX = 2;
			break;

		case EXPL_END_RIGHT:
			clipX = 4;
			break;
	}

	logger->dbg("-- clip: x: %d, y: 1", clipX);

	part->clip->x = clipX * BOMB_SIZE;

	part->clip->y = BOMB_SIZE;
	part->clip->h = BOMB_SIZE;
	part->clip->w = BOMB_SIZE;

	return part;
}

void placeExplosion(Bomb* bomb) {
	static int cnt  = 0;
	AssetMgr* ast = getAssets();

	bomb->state++;
	int y = bomb->state - 5;

	char name[45];
	snprintf(name, 35, "Explosion-%d", cnt++);

	Object* obj = addSimpleObject(name, ast->getImg("bomb3"), NULL, 2);
	bomb->obj = obj;

	obj->pos.x = bomb->pos.x;
	obj->pos.y = bomb->pos.y;

	bomb->clip.x = 0;
	bomb->clip.y = BOMB_SIZE;
	bomb->clip.h = BOMB_SIZE;
	bomb->clip.w = BOMB_SIZE;

	obj->clip = &bomb->clip;
	obj->container = bomb;
	obj->containerType = BOMB;

	int colideWith = COL_WALL ;// | COL_PLAYER;

	SDL_Rect hit = {2,2, BOMB_SIZE - 5, BOMB_SIZE - 5};
	setHitBox(obj, hit, 0, COL_BOMB);
	obj->collision->fnc = explosionHit;
	obj->collision->colFlags = colideWith;

	short z;
	short x;
	short dir;

	short targetX = 0;
	short targetY = 0;

	ExplosionPart part;
	Object* objPart = NULL;
	for (dir = UP; dir <= LEFT; ++dir) {
		for (z = 1; z < bomb->power; ++z){
			switch (dir) {
				case UP:
					x = 0;
					y = -(z);
					part = EXPL_MID_V;

					targetX = obj->pos.x;
					targetY = obj->pos.y - (z * BOMB_SIZE);
					break;

				case DOWN:
					x = 0;
					y = z;
					part = EXPL_MID_V;

					targetX = obj->pos.x;
					targetY = obj->pos.y + (z * BOMB_SIZE);
					break;

				case RIGHT:
					y = 0;
					x = z;
					part = EXPL_MID_H;

					targetY = obj->pos.y;
					targetX = obj->pos.x + (z * BOMB_SIZE);
					break;

				case LEFT:
					y = 0;
					x = -(z);
					part = EXPL_MID_H;

					targetY = obj->pos.y;
					targetX = obj->pos.x - (z * BOMB_SIZE);
					break;
			}

			if (!canMoveTo(obj, targetX, targetY)) {
				break;
			}

			objPart = genExplosionPart(part, x, y);
			addChild(obj, objPart);
			setHitBox(objPart, hit, 0, COL_BOMB);
			objPart->collision->fnc = explosionHit;
			obj->collision->colFlags = colideWith;
		}

		switch (dir) {
			case UP:
				x = 0;
				y = -(z);
				part = EXPL_END_TOP;
				break;

			case DOWN:
				x = 0;
				y = z;
				part = EXPL_END_DOWN;
				break;

			case RIGHT:
				y = 0;
				x = z;
				part = EXPL_END_RIGHT;
				break;

			case LEFT:
				y = 0;
				x = -(z);
				part = EXPL_END_LEFT;
				break;
		}

		objPart = genExplosionPart(part, x, y);
		addChild(obj, objPart);
		setHitBox(objPart, hit, 0, COL_BOMB);
		objPart->collision->fnc = explosionHit;
		obj->collision->colFlags = colideWith;
	}


	SDL_Rect animClip = {0, (y+1) * BOMB_SIZE, BOMB_SIZE, BOMB_SIZE};
	AnimParam* anim = spriteAnim(obj, animClip, 0.1f, 0, 1);

	anim->deleteObject = 0;
	anim->callBack = iterateExplosion;

	bomb->exploded = 1;
}

void bombExplode(Object* obj) {
	logger->inf("=== BOMB EXPLODE ===");

	Bomb* bomb = obj->container;
	obj->container = NULL;
	obj->containerType = NONE;

	placeExplosion(bomb);
}

void iterateBomb(AnimParam* anim) {
	Object* obj = (Object*) anim->obj;
	Bomb* bomb = (Bomb*) obj->container;

	if (obj == NULL || bomb == NULL) {
		return;
	}

	bomb->state++;

	int posY = 0;

	switch (bomb->state) {
		case 1:
			posY = bomb->pos.y;
			break;

		case 2:
			logger->dbg("=== RESET Can Place");
			bomb->owner->canPlaceBomb = 1;
			posY = bomb->pos.y -10;
			break;

		case 3:
			posY = bomb->pos.y;
			break;
	}


	if (bomb->state > 3){
		posY = bomb->pos.y;
		bomb->clip.y = 1;

		/*if (bomb->state == 4)
		{
			logger->dbg("-- Add Collision");
			SDL_Rect hit = {0,0,BOMB_SIZE, BOMB_SIZE};
			setHitBox(obj, hit, 0, COL_BOMB);

			obj->collision->colFlags = COL_PLAYER | COL_WALL;
			obj->collision->fnc = bombHit;
		}*/

		if (bomb->state < 6) {
			if (bomb->clip.x >= BOMB_SIZE){
				bomb->clip.x -= BOMB_SIZE;
			}
		}
		else if(bomb->state == 16){
			bomb->clip.x = BOMB_SIZE * 3;
		}
	}

	if (bomb->state < 7) {
		logger->dbg("Bomb Anim %s", obj->name);
		anim = moveTo(obj, bomb->pos.x, posY, 0.2f, 0);

		if (anim != NULL) {
			logger->dbg("Bomb Anim Success");
			anim->callBack = iterateBomb;
		}
	}
	else{
		obj->lifetime = 0; //BOMB_TIME;
		obj->onDelete = bombExplode;
	}
}

void placeBomb(Player* p) {
	static int cnt = 0;
	enableLogger(DBG_BOMB);

	AssetMgr* ast = getAssets();

	if (p->bombCnt <= 0 || !p->canPlaceBomb) {
		return;
	}

	p->bombCnt--;
	p->canPlaceBomb = 0;

	logger->dbg("==== PLACE BOMB ====");
	Bomb* bomb = malloc(sizeof(Bomb));

	logger->dbg("-- Add Object");

	char name[35];
	snprintf(name, 35, "Bomb-%d", cnt++);

	logger->dbg("-- Object Name: %s", name);
	Object* obj = addSimpleObject(name, ast->getImg("bomb3"), NULL, 2);


	logger->dbg("-- Setting Pos");
	obj->pos.x = p->object->pos.x + (PLAYER_W / 2) - 20;
	obj->pos.y = p->object->pos.y + (PLAYER_H / 2);

	logger->dbg("-- Setting Clip");
	bomb->owner = p;
	bomb->state = 0;
	bomb->clip.y = 0;
	bomb->exploded = 0;
	bomb->direction = UP;
	bomb->moveIterate = 0;
	bomb->clip.x = BOMB_SIZE * 2;


	bomb->clip.w = BOMB_SIZE;
	bomb->clip.h = BOMB_SIZE;
	bomb->power = p->bombPower;

	logger->dbg("-- Setting Anim Pos");
	bomb->pos.x = p->object->pos.x + (PLAYER_W / 2) - 20;
	bomb->pos.y = p->object->pos.y + (PLAYER_H - 30);

	logger->dbg("-- Link Clip");
	obj->clip = &bomb->clip;

	obj->container = bomb;
	obj->containerType = BOMB;
	bomb->obj = obj;

	logger->dbg("-- Animate");
	AnimParam* anim = moveTo(obj, bomb->pos.x, bomb->pos.y - 10, 0.1f, 0);
	anim->callBack = iterateBomb;

	/*
	logger->dbg("-- Add Collision");
	SDL_Rect hit = {0,0,BOMB_SIZE, BOMB_SIZE};
	setHitBox(obj, hit, 0, COL_BOMB);

	obj->collision->colFlags = COL_PLAYER | COL_WALL;
	obj->collision->enabled = 0;
	obj->collision->fnc = NULL;
	*/

	logger->dbg("==== PLACE BOMB END ====");
}
