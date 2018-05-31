#include "player.h"

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

	while((n = listIterate(players, n)) != NULL) {
		logger->inf("--Delete Node: %p", n);
	    deletePlayer(n->value);
	    deleteNode(players, n->id);

	    n = NULL;
	}

	logger->dbg("==== CLEAR PLAYER LIST DONE ====");
}

void calcPlayerPos(SDL_Rect objPos, SDL_Rect* playPos) {
	// playPos.x = (CELL_SIZE * playPos.x) + 27;
	// playPos.y = (CELL_SIZE * playPos.y) + 27;

	playPos->x = objPos.x - MAP_X;
	playPos->y = objPos.y - (MAP_Y + PLAYER_H);
}

void calcPlayerObjectPos(SDL_Rect playPos, SDL_Rect* objPos) {
	// playPos.x = (CELL_SIZE * playPos.x) + 27;
	// playPos.y = (CELL_SIZE * playPos.y) + 27;

	objPos->x = (MAP_X + playPos.x);
	objPos->y = MAP_Y + playPos.y - PLAYER_H;
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
	logger->inf("==== DELETING PLAYER: %s ====", p->name);
	free(p->name);

	deleteObject(p->object);
	logger->dbg("==== DELETE PLAYER DONE ====");
}

void updatePlayerClip(Player* p) {
	if (p->clipIndex > 2) {
		p->clipIndex = 0;
	}


	if (!p->alive) {
		p->clip.x = PLAYER_W * 3;
		p->clip.y = PLAYER_H;
	}
	else{
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

	logger->err("Player Clip Index: %d", p->clipIndex);
	logger->err("Player Clip: x: %d, y: %d, w: %d, h: %d, ", p->clip.x, p->clip.y, p->clip.w, p->clip.h);
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
	p->direction = DOWN;
	
	p->pos.x = 0;
	p->pos.y = 0;

	logger->dbg("-- loading image");
	AssetMgr* ast = getAssets();
	SDL_Surface* img = ast->getImg("player");

	logger->dbg("-- Creating Object");
	p->object = addSimpleObject(name, img, NULL, 2);
	
	p->clip.w = PLAYER_W;
	p->clip.h = PLAYER_H;

	updatePlayerClip(p);
	p->object->clip = &p->clip;

	p->clipIndex = 0;
	p->object->clipIndex = &p->clipIndex;
	
	p->object->container = p;

	calcPlayerObjectPos(p->pos, &p->object->pos);

	ListManager* players = getPlayerList();
	addNodeV(players, name, p, 0);

	logger->inf("==== GEN PLAYER: %s DONE ====", name);

	return p;
}

void playerTickMove(AnimParam* anim) {
	Player* p = (Player*) anim->obj->container;
	playerMove(p, p->direction);
}

void playerMove(Player* p, short direction) {
	p->direction = direction;

	logger->err("Player: %s", p->name);
	logger->err("Pos: x:%d, y:%d", p->object->pos.x, p->object->pos.y);

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

	logger->err("Moving To x:%d, y:%d", p->object->pos.x + moveX, p->object->pos.y + moveY);
	AnimParam* anim = moveTo(p->object, p->object->pos.x + moveX, p->object->pos.y + moveY, 0.3f, 0);
	
	anim->callBack = playerTickMove;
}

void placeExplosion(Bomb* bomb) {
	AssetMgr* ast = getAssets();
	SDL_Surface* img = ast->getImg("bomb");
	
	if(img == NULL) {
		logger->err("Faild To Get Image");
		return;
	}

	bomb->state++;
	int y =  bomb->state - 3;
	Object* obj = addSimpleObject("Bomb", ast->getImg("bomb"), NULL, 2);

	obj->pos.x = bomb->pos.x;
	obj->pos.y = bomb->pos.y;

	bomb->clip.x = 0;
	bomb->clip.y = y * 24;
	bomb->clip.h = 24;
	bomb->clip.w = 24;
	
	obj->clip = &bomb->clip;
	
	Object* part1 = genSimpleObject("Bomb", ast->getImg("bomb"), NULL, 2);
	part1->pos.y = 0;
	part1->pos.x = -24;
	

	part1->clip = malloc(sizeof(SDL_Rect));
	part1->clip->x = 24;
	part1->clip->y = y * 24;

	part1->clip->h = 24;
	part1->clip->w = 24;
	
	addChild(obj, part1);
}

void iterateBomb(AnimParam* anim) {
	Object* obj = (Object*) anim->obj;
	Bomb* bomb = (Bomb*) obj->container;


	bomb->state++;
	logger->err("BOMB STATE %d", bomb->state);
	
	int posY = -100;
	short deleteObject = 0;

	switch (bomb->state) {
		case 1:
			posY = bomb->pos.y;
			break;
		
		case 3:
			posY = bomb->pos.y;
			deleteObject = 1;
			break;

		case 2:
			posY = bomb->pos.y -10;
			break;
	}

	if (posY != -100) {
		logger->err("PosY: %d", posY);
		anim = moveTo(obj, bomb->pos.x, posY, 0.3f, 0);
		
		anim->callBack = iterateBomb;
		anim->deleteObject = deleteObject;
	}
	else{
		free(bomb);
		//placeExplosion(bomb);
	}
}

void placeBomb(Player* p ) {
	AssetMgr* ast = getAssets();

	logger->err("Add Object");
	SDL_Surface* img = ast->getImg("bomb");
	
	if(img == NULL) {
		logger->err("Faild To Get Image");
		return;
	}

	Object* obj = addSimpleObject("Bomb", ast->getImg("bomb"), NULL, 2);
	
	logger->err("Malloc Bomb");
	Bomb* bomb = malloc(sizeof(Bomb));

	logger->err("Setting Pos");
	obj->pos.x = p->object->pos.x + (PLAYER_W / 2);
	obj->pos.y = p->object->pos.y + (PLAYER_H / 2);

	logger->err("Setting Clip");
	bomb->state = 0;
	bomb->clip.x = 0;
	bomb->clip.y = 0;
	bomb->clip.w = 24;
	bomb->clip.h = 24;

	bomb->pos.x = p->object->pos.x + (PLAYER_W / 2);
	bomb->pos.y = p->object->pos.y + (PLAYER_H - 10);

	logger->err("Link Clip");
	obj->clip = &bomb->clip;
	
	obj->container = bomb;
	bomb->obj = obj;

	AnimParam* anim = moveTo(obj, bomb->obj->pos.x, bomb->obj->pos.y - 10, 0.3f, 0);
	anim->callBack = iterateBomb;
}