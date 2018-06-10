#include "bomb.h"

void explosionHit(Object* explObj, Object* targetObj) {
	Game* game = getGame();
	logger->enabled = game->flags & DBG_BOMB;


	logger->dbg("EXPLOSION COLLISION !!!");
	logger->dbg("Explosion: %s !!!", explObj->name);
	logger->dbg("Target: %s !!!", targetObj->name);

	if (targetObj->container == NULL) {
		return;
	}

	switch(targetObj->containerType) {
		case PLAYER:
			killPlayer(targetObj->container);
			break;

		case BLOCK:
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
	Game* game = getGame();
	logger->enabled = game->flags & DBG_BOMB;
	
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
	Game* game = getGame();
	logger->enabled = game->flags & DBG_BOMB;

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
		
		anim = spriteAnim(anim->obj, animClip, 0.1f, 0, 1);
		anim->callBack = iterateExplosion;
	}
}

Object* genExplosionPart(ExplosionPart index, int x, int y) {
	Game* game = getGame();
	logger->enabled = game->flags & DBG_BOMB;
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

	SDL_Rect hit = {0,0, BOMB_SIZE, BOMB_SIZE};
	setHitBox(obj, hit, 0, 1);
	obj->collision->fnc = explosionHit;

	short z;
	short x;
	short dir;

	short targetX = 0;
	short targetY = 0;

	ExplosionPart part;
	Object* objPart = NULL;
	for (dir = UP; dir <= LEFT; ++dir) {
		for (z = 1; z <= bomb->power; ++z){
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
			setHitBox(objPart, hit, 0, 1);
			objPart->collision->fnc = explosionHit;
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
		setHitBox(objPart, hit, 0, 1);
		objPart->collision->fnc = explosionHit;
	}


	SDL_Rect animClip = {0, (y+1) * BOMB_SIZE, BOMB_SIZE, BOMB_SIZE};
	AnimParam* anim = spriteAnim(obj, animClip, 0.1f, 0, 1);
	
	anim->deleteObject = 0;
	anim->callBack = iterateExplosion;
}

void iterateBomb(AnimParam* anim) {
	Object* obj = (Object*) anim->obj;
	Bomb* bomb = (Bomb*) obj->container;

	bomb->state++;
	
	int posY = -100;

	switch (bomb->state) {
		case 1:
			posY = bomb->pos.y;
			break;
		
		case 3:
			posY = bomb->pos.y;
			break;

		case 2:
			posY = bomb->pos.y -10;
			break;
	}

	if (bomb->state > 3){
		posY = bomb->pos.y;
		bomb->clip.y = 1;

		if (bomb->state < 6) {
			bomb->clip.x -= BOMB_SIZE;
		}
		else{
			bomb->clip.x = BOMB_SIZE * 3;
		}
	}
	
	if (bomb->state < 7) {
		anim = moveTo(obj, bomb->pos.x, posY, 0.2f, 0);
		
		anim->callBack = iterateBomb;
	}
	else{
		anim->deleteObject = 1;
		
		obj->container = NULL;
		obj->containerType = NONE;

		placeExplosion(bomb);
	}
}

void placeBomb(Player* p) {
	static int cnt = 0;
	Game* game = getGame();
	logger->enabled = game->flags & DBG_BOMB;
	
	AssetMgr* ast = getAssets();

	logger->inf("==== PLACE BOMB ====");
	Bomb* bomb = malloc(sizeof(Bomb));

	logger->dbg("-- Add Object");

	char name[35];
	snprintf(name, 35, "Bomb-%d", cnt++);
	Object* obj = addSimpleObject(name, ast->getImg("bomb3"), NULL, 2);
	

	logger->dbg("-- Setting Pos");
	obj->pos.x = p->object->pos.x + (PLAYER_W / 2) - 20;
	obj->pos.y = p->object->pos.y + (PLAYER_H / 2);

	logger->dbg("-- Setting Clip");
	bomb->state = 0;
	bomb->clip.y = 0;
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

	logger->dbg("==== PLACE BOMB END ====");
}