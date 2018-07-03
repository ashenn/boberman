#include "player.h"

void resetPlayersBomb() {
	Node* n = NULL;
	ListManager* players = getPlayerList();

	while((n = listIterate(players, n)) != NULL) {
	    Player* p = (Player*) n->value;

	    if (p == NULL || !p->alive) {
	    	continue;
	    }

	    p->bombCnt = p->bombMax;
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

	enableLogger(DBG_PLAYER);

	logger->inf("==== Init Player List ====");
	players = initListMgr();
	return players;
}

void clearPlayers() {
	enableLogger(DBG_PLAYER);

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
	enableLogger(DBG_PLAYER);

	logger->inf("==== DELETING PLAYER: %s ====", p->name);
	free(p->name);

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
		if (p->clipIndex > 4) {
			p->clipIndex = 0;
		}

		p->clip.y = 0;
		int index = (p->clipIndex / 2) + ((p->clipIndex % 2) > 0.5f);

		if (p->direction == DOWN || p->direction == RIGHT) {
			p->clip.x = index * PLAYER_W;
			p->clip.y = p->direction == RIGHT ? PLAYER_H : 0;
		}
		else if(p->direction == UP){
			p->clip.x = (PLAYER_ANIM_LEN + index) * PLAYER_W;
		}
		else {
			p->clip.x = ((PLAYER_ANIM_LEN * 2) + index) * PLAYER_W;
		}
	}

	enableLogger(DBG_PLAYER);

	logger->dbg("Player Clip Index: %d", p->clipIndex);
	logger->dbg("Player Clip: x: %d, y: %d, w: %d, h: %d, ", p->clip.x, p->clip.y, p->clip.w, p->clip.h);
}

Player* genPlayer(char* name) {

	static int cnt = 0;
	if(cnt >= 4) {
		return NULL;
	}

	logger->inf("#### GEN PLAYER %s/%d", name, cnt);

	enableLogger(DBG_PLAYER);

	logger->inf("==== GEN PLAYER: %s ====", name);
	Player* p = malloc(sizeof(Player));

	logger->dbg("-- Player Address %p", p);

	if (p == NULL) {
		logger->err("#### Fail To Malloc Player");
		logger->dbg("==== GEN PLAYER FAILD ====");
		return NULL;
	}

	p->name = malloc(strlen(name)+1);
	strcpy(p->name, name);

	p->alive = 1;
	p->shoot = 0;
	p->speed = 1.0f;
	p->bombCnt = 1;
	p->bombMax = 1;
	p->bombPower = 1;
	p->canPlaceBomb = 1;
	p->direction = DOWN;


	p->pos.x = 0;
	p->pos.y = 0;
	switch(cnt) {
		case 1:
			p->pos.x = MAP_W - 1;
			break;

		case 2:
			p->direction = UP;
			p->pos.y = MAP_H-1;
			break;

		case 3:
			p->direction = UP;
			p->pos.y = MAP_H-1;
			p->pos.x = MAP_W - 1;
			break;

	}

	p->clipIndex = 0;

	logger->dbg("-- loading image");
	AssetMgr* ast = getAssets();
	SDL_Surface* img = ast->getImg("player");

	logger->dbg("-- Creating Object");
	SDL_Rect pos = {0,0, PLAYER_W, PLAYER_H};
	Object* obj = p->object = addSimpleObject(name, img, &pos, 3);


	p->clip.w = PLAYER_W;
	p->clip.h = PLAYER_H;

	p->object->clip = &p->clip;
	obj->clipIndex = &p->clipIndex;

	obj->container = p;
	obj->containerType = PLAYER;


	calcPlayerObjectPos(p->pos, &p->object->pos);
	updatePlayerClip(p);

	SDL_Rect hitrect = {18, 60, 20, 15};
	setHitBox(obj, hitrect, 1, COL_PLAYER);
	obj->collision->colFlags = COL_ALL;

	ListManager* players = getPlayerList();
	Node* n = addNodeV(players, name, p, 0);
	p->id = n->id;

	logger->inf("==== GEN PLAYER: %s DONE ====", name);

	cnt++;
	return p;
}

void playerTickMove(AnimParam* anim) {
	Player* p = (Player*) anim->obj->container;
	playerMove(p, p->direction);
}

void playerStop(Player* p) {

	Game* game = getGame();
	/*if(game->isServer) {
		char msg[25];
		memset(msg, 0, 25); 
		snprintf(msg, 25, "stop:%d", p->id);
		broadcast(msg, getClient());
	}*/
	
	p->clipIndex = 0;
	animRemoveObject(p->object);
	updatePlayerClip(p);
}

void broadcastStop(Player* p) {
	logger->dbg("!!!!!! BROADCAST STOOOP !!!!!!!");
	Game* game = getGame();

	if(game->isServer) {
		char msg[25];
		memset(msg, 0, 25); 
		snprintf(msg, 25, "stop:%d", p->id);

		broadcast(msg, NULL);
	}
}

void broadcastMove(Player* p, int direction) {
	logger->dbg("!!!!!! BROADCAST MOOOVE !!!!!!!");
	Game* game = getGame();
	if(game->isServer) {
		char msg[25];
		memset(msg, 0, 25); 
		snprintf(msg, 25, "move:%d:%d", p->id, direction);
		broadcast(msg, NULL);
	}
}

void* playerMove(Player* p, short direction) {
	if(p == NULL || !p->alive) {
		return NULL;
	}


	enableLogger(DBG_PLAYER);

	p->direction = direction;

	logger->dbg("Player: %s", p->name);
	logger->dbg("Pos: x:%d, y:%d", p->object->pos.x, p->object->pos.y);

	int moveX = 0;
	int moveY = 0;

	switch (direction) {
		case UP:
			moveY = -10 * p->speed;
			break;

		case DOWN:
			moveY = 10 * p->speed;
			break;

		case RIGHT:
			moveX = 10 * p->speed;
			break;

		case LEFT:
			moveX = -10 * p->speed;
			break;
	}

	p->clipIndex++;
	updatePlayerClip(p);

	logger->dbg("Moving To x:%d, y:%d", p->object->pos.x + moveX, p->object->pos.y + moveY);
	AnimParam* anim = moveTo(p->object, p->object->pos.x + moveX, p->object->pos.y + moveY, 0.07f, 0);

	if (anim == NULL) {
		logger->dbg("CANT MOVE TO POS: x:%d, y:%d", p->object->pos.x + moveX, p->object->pos.y + moveY);
		return NULL;
	}

	anim->callBack = playerTickMove;
}

short iteratePlayerKill(AnimParam* anim) {
	enableLogger(DBG_PLAYER);

	logger->dbg("==== Iterate Player Kill ====");
	Object* obj = (Object*) anim->obj;
	Player* p = (Player*) obj->container;

	p->clipIndex++;

	updatePlayerClip(p);

	if (p->clipIndex > 6) {
		logger->dbg("=== PLAYER ILL DONE DELETING");
		//anim->deleteObject = 1;
		obj->enabled = 0;
		obj->visible = 0;
		return 0;
	}

	logger->dbg("==== Iterate Player Kill DONE ====");
	return 1;
}

void killPlayer(Player* p) {
	if (!p->alive)
		return;

	enableLogger(DBG_PLAYER);

	logger->war("==== KILL PLAYER ====");
	logger->war("-- Player #%d: %s", p->id, p->name);

	animRemoveObject(p->object);

	p->alive = 0;
	p->clipIndex = 0;
	updatePlayerClip(p);
	p->object->collision->enabled = 0;


	AnimParam* anim = customAnim(p->object, 0.1f, 0, iteratePlayerKill);

	Game* game = getGame();
	if(!game->isServer) {
		return;
	}
	
	p = NULL;
	Node* n = NULL;
	int alives = 0;
	ListManager* players = getPlayerList();
	while((n = listIterate(players, n)) != NULL) {
		p = n->value;
		alives += p->alive;
	}

	if(alives <= 1) {
		changeGameStatus(GAME_END);
	}
}
