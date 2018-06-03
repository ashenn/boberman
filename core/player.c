#include "player.h"

void playerHit(Object* o1, Object*o2) {

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

Player* initPlayer(Player* p) {
	static Player* player = NULL;
	if (p != NULL) {
		player = p;
	}

	return player;
}

Player* getPlayer() {
	return initPlayer(NULL);
}

ListManager* getPlayerList() {
	static ListManager* players = NULL;
	
	if (players != NULL){
		return players;
	}

	logger->inf("==== Init Object List ====");
	players = initListMgr();
	return players;
}

void clearPlayers() {
	logger->inf("==== CLEAR PLAYER LIST ====");
	ListManager* players = getPlayerList();
	Node* n = NULL;

	Player* p = NULL;
	while((n = listIterate(players, n)) != NULL) {
		logger->inf("--Delete Node: %p", n);
	    
	    p = (Player*) n->value;
	    deleteObject(p->object);

	    n = NULL;
	}

	logger->dbg("==== CLEAR PLAYER LIST DONE ====");
}

void calcPlayerPos(SDL_Rect objPos, SDL_Rect* playPos) {
	playPos->x = ((objPos.x - MAP_X) + CELL_SIZE / 2) / CELL_SIZE;
	playPos->y = (
			(
				(
					objPos.y -
					MAP_Y
				)
				+ PLAYER_H
			)
			-15

		) / CELL_SIZE
	;
	playPos->y++;
	//logger->err("Player Pos: X: %d, Y: %d", playPos->x, playPos->y);
}

void calcPlayerObjectPos(SDL_Rect playPos, SDL_Rect* objPos) {
	objPos->x = (MAP_X + (playPos.x * CELL_SIZE));
	objPos->y = MAP_Y + ((playPos.y + 1) * CELL_SIZE) - PLAYER_H;
	//logger->err("Player Object Pos: X: %d, Y: %d", objPos->x, objPos->y);
}

void* updatePlayers() {
	Node* n = NULL;
	ListManager* players = getPlayerList();

	while((n = listIterate(players, n)) != NULL) {
		Player* player = n->value;
	    calcPlayerPos(player->object->pos, &player->pos);
	}
}

void* deletePlayer(Player* p) {
	logger->err("==== DELETING PLAYER: %s ====", p->name);
	free(p->name);

	/*
	if (p->object->hitObj != NULL) {
		logger->err("-- delete HitObject");
		deleteObject(p->object->hitObj);
	}
	*/

	ListManager* players = getPlayerList();
	deleteNode(players, p->id);

	logger->dbg("==== DELETE PLAYER DONE ====");
}

void updatePlayerClip(Player* p) {
	calcPlayerPos(p->object->pos, &p->pos);

	if (!p->alive) {
		if (p->clipIndex < 5) {
			p->clip.y = PLAYER_H;
			p->clip.x = (PLAYER_ANIM_LEN * PLAYER_W) + (p->clipIndex * PLAYER_W);
		}
		else{
			p->clip.y = PLAYER_H * 3;
			p->clip.w = (PLAYER_W * 2);

			if (p->clipIndex == 5) {
				p->object->pos.x -= PLAYER_W / 2;
			}

			p->clip.x = (PLAYER_ANIM_LEN * PLAYER_W) + ((p->clipIndex - 5) * (PLAYER_W * 2));
		}
	}
	else{
		if (p->clipIndex > 2) {
			p->clipIndex = 0;
		}

		p->clip.y = 0;
		
		if (p->direction == DOWN || p->direction == RIGHT) {
			p->clip.x = p->clipIndex * PLAYER_W;
			p->clip.y = p->direction == RIGHT ? PLAYER_H : 0;
		}
		else if(p->direction == UP){
			p->clip.x = (PLAYER_ANIM_LEN + p->clipIndex) * PLAYER_W;
		}
		else {
			p->clip.x = ((PLAYER_ANIM_LEN * 2) + p->clipIndex) * PLAYER_W;
		}
	}

	logger->dbg("Player Clip Index: %d", p->clipIndex);
	logger->dbg("Player Clip: x: %d, y: %d, w: %d, h: %d, ", p->clip.x, p->clip.y, p->clip.w, p->clip.h);
}

Player* genPlayer(char* name) {
	logger->inf("==== GEN PLAYER: %s ====", name);
	Player* p = malloc(sizeof(Player));
	if (p == NULL) {
		logger->err("#### Fail To Malloc Player");
		logger->dbg("==== GEN PLAYER FAILD ====");
		return NULL;
	}

	p->name = malloc(strlen(name)+1);
	strcpy(p->name, name);

	p->alive = 1;
	p->bombPower = 1;
	p->direction = DOWN;
	
	p->pos.x = 0;
	p->pos.y = 0;

	logger->dbg("-- loading image");
	AssetMgr* ast = getAssets();
	SDL_Surface* img = ast->getImg("player");

	logger->dbg("-- Creating Object");
	SDL_Rect pos = {0,0, PLAYER_W, PLAYER_H};
	Object* obj = p->object = addSimpleObject(name, img, &pos, 3);
	
	p->clipIndex = 0;
	p->clip.w = PLAYER_W;
	p->clip.h = PLAYER_H;

	p->object->clip = &p->clip;
	obj->clipIndex = &p->clipIndex;
	
	obj->container = p;
	obj->containerType = PLAYER;

	
	calcPlayerObjectPos(p->pos, &p->object->pos);
	updatePlayerClip(p);
	
	SDL_Rect hitrect = {10, 50, 36, 30};
	obj->hit = playerHit;
	setHitBox(obj, hitrect);

	ListManager* players = getPlayerList();
	Node* n = addNodeV(players, name, p, 0);
	p->id = n->id;

	logger->inf("==== GEN PLAYER: %s DONE ====", name);

	return p;
}

void playerTickMove(AnimParam* anim) {
	Player* p = (Player*) anim->obj->container;
	playerMove(p, p->direction);
}

void playerMove(Player* p, short direction) {
	p->direction = direction;

	logger->dbg("Player: %s", p->name);
	logger->dbg("Pos: x:%d, y:%d", p->object->pos.x, p->object->pos.y);

	int moveX = 0;
	int moveY = 0;

	switch (direction) {
		case UP:
			moveY = -20;
			break;

		case DOWN:
			moveY = 20;
			break;

		case RIGHT:
			moveX = 20;
			break;

		case LEFT:
			moveX = -20;
			break;
	}

	p->clipIndex++;
	updatePlayerClip(p);

	logger->dbg("Moving To x:%d, y:%d", p->object->pos.x + moveX, p->object->pos.y + moveY);
	AnimParam* anim = moveTo(p->object, p->object->pos.x + moveX, p->object->pos.y + moveY, 0.3f, 0);
	
	anim->callBack = playerTickMove;
}



void clearExplosion(Object* obj) {
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
	logger->err("==== ITARATE EXPLOSION ====");
	if (anim->obj->container == NULL) {
		logger->err("Explosion Container Is Empty");
		return;
	}

	Bomb* bomb = (Bomb*) anim->obj->container;
	bomb->state++;
	int y =  bomb->state - 9;
	logger->err("-- Explosion State: %d", y);
	
	if (y > 3){
		logger->err("-- Delete Object");
		anim->deleteObject = 1;
		//clearExplosion(anim->obj);
	}
	else{
		logger->err("-- Adjust Clip");
		SDL_Rect animClip = {0, (y+1) * BOMB_SIZE, BOMB_SIZE, BOMB_SIZE};
		anim = spriteAnim(anim->obj, animClip, 0.3f, 0, 1);
		anim->callBack = iterateExplosion;
	}
}

Object* genExplosionPart(ExplosionPart index, int x, int y) {
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

	short z;
	short x;
	short dir;
	ExplosionPart part;
	Object* objPart = NULL;
	for (dir = UP; dir <= LEFT; ++dir) {
		for (z = 0; z < bomb->power; ++z){
			switch (dir) {
				case UP:
					x = 0;
					y = -(z + 1);
					part = EXPL_MID_V;
					break; 

				case DOWN:
					x = 0;
					y = z + 1;
					part = EXPL_MID_V;
					break; 

				case RIGHT:
					y = 0;
					x = z + 1;
					part = EXPL_MID_H;
					break; 

				case LEFT:
					y = 0;
					x = -(z + 1);
					part = EXPL_MID_H;
					break; 

			}

			objPart = genExplosionPart(part, x, y);
			addChild(obj, objPart);
		}

		switch (dir) {
			case UP:
				x = 0;
				y = -(z + 1);
				part = EXPL_END_TOP;
				break; 

			case DOWN:
				x = 0;
				y = z + 1;
				part = EXPL_END_DOWN;
				break; 

			case RIGHT:
				y = 0;
				x = z + 1;
				part = EXPL_END_RIGHT;
				break; 

			case LEFT:
				y = 0;
				x = -(z + 1);
				part = EXPL_END_LEFT;
				break; 
		}

		objPart = genExplosionPart(part, x, y);
		addChild(obj, objPart);
	}


	SDL_Rect animClip = {0, (y+1) * BOMB_SIZE, BOMB_SIZE, BOMB_SIZE};
	AnimParam* anim = spriteAnim(obj, animClip, 0.2f, 0, 1);
	
	anim->deleteObject = 0;
	anim->callBack = iterateExplosion;
}

void iterateBomb(AnimParam* anim) {
	Object* obj = (Object*) anim->obj;
	Bomb* bomb = (Bomb*) obj->container;


	bomb->state++;
	logger->dbg("BOMB STATE %d", bomb->state);
	
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
		logger->dbg("PosY: %d", posY);
		anim = moveTo(obj, bomb->pos.x, posY, 0.3f, 0);
		
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
	AssetMgr* ast = getAssets();

	logger->dbg("Malloc Bomb");
	Bomb* bomb = malloc(sizeof(Bomb));

	logger->dbg("Add Object");

	char name[35];
	snprintf(name, 35, "Bomb-%d", cnt++);
	Object* obj = addSimpleObject(name, ast->getImg("bomb3"), NULL, 2);
	

	logger->dbg("Setting Pos");
	obj->pos.x = p->object->pos.x + (PLAYER_W / 2) - 10;
	obj->pos.y = p->object->pos.y + (PLAYER_H / 2);

	logger->dbg("Setting Clip");
	bomb->state = 0;
	bomb->clip.y = 0;
	bomb->clip.x = BOMB_SIZE * 2;

	bomb->clip.w = BOMB_SIZE;
	bomb->clip.h = BOMB_SIZE;
	bomb->power = p->bombPower;

	logger->dbg("Setting Anim Pos");
	bomb->pos.x = p->object->pos.x + (PLAYER_W / 2) - 10;
	bomb->pos.y = p->object->pos.y + (PLAYER_H - 10);

	logger->dbg("Link Clip");
	obj->clip = &bomb->clip;
	
	obj->container = bomb;
	obj->containerType = BOMB;
	bomb->obj = obj;

	AnimParam* anim = moveTo(obj, bomb->pos.x, bomb->pos.y - 10, 0.3f, 0);
	anim->callBack = iterateBomb;
}

short iteratePlayerKill(AnimParam* anim) {
	logger->err("==== Iterate Player Kill ====");
	Object* obj = (Object*) anim->obj;
	Player* p = (Player*) obj->container;

	++p->clipIndex;

	updatePlayerClip(p);

	if (p->clipIndex > 6) {
		logger->err("=== PLAYER ILL DONE DELETING");
		anim->deleteObject = 1;
		return 0;
	}

	logger->err("==== Iterate Player Kill DONE ====");
	return 1;
}

void killPlayer(Player* p) {
	logger->err("==== KILL PLAYER ====");
	logger->err("-- Player: %s", p->name);
	
	p->alive = 0;
	p->clipIndex = 0;
	updatePlayerClip(p);

	AnimParam* anim = customAnim(p->object, 0.3f, 0, iteratePlayerKill);
}