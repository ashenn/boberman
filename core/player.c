#include "player.h"

void playerHit(Object* o1, Object*o2) {

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

	logger->dbg("Player Clip Index: %d", p->clipIndex);
	logger->dbg("Player Clip: x: %d, y: %d, w: %d, h: %d, ", p->clip.x, p->clip.y, p->clip.w, p->clip.h);
}

Player* genPlayer(char* name) {
	logger->inf("==== GEN PLAYER: %s ====", name);
	Player* p = malloc(sizeof(Player));

	logger->enabled = 1;
	logger->err("-- Player Address %p", p);
	
	if (p == NULL) {
		logger->err("#### Fail To Malloc Player");
		logger->dbg("==== GEN PLAYER FAILD ====");
		return NULL;
	}

	p->name = malloc(strlen(name)+1);
	strcpy(p->name, name);

	p->alive = 1;
	p->bombPower = 3;
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
	
	SDL_Rect hitrect = {18, 60, 20, 15};
	setHitBox(obj, hitrect, 1);

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
			moveY = -10;
			break;

		case DOWN:
			moveY = 10;
			break;

		case RIGHT:
			moveX = 10;
			break;

		case LEFT:
			moveX = -10;
			break;
	}

	p->clipIndex++;
	updatePlayerClip(p);

	logger->dbg("Moving To x:%d, y:%d", p->object->pos.x + moveX, p->object->pos.y + moveY);
	AnimParam* anim = moveTo(p->object, p->object->pos.x + moveX, p->object->pos.y + moveY, 0.15f, 0);

	if (anim == NULL) {
		logger->dbg("CANT MOVE TO POS: x:%d, y:%d", p->object->pos.x + moveX, p->object->pos.y + moveY);
		return;
	}
	
	anim->callBack = playerTickMove;
}

short iteratePlayerKill(AnimParam* anim) {
	logger->enabled = 1;
	logger->err("==== Iterate Player Kill ====");
	Object* obj = (Object*) anim->obj;
	Player* p = (Player*) obj->container;

	p->clipIndex++;

	updatePlayerClip(p);

	if (p->clipIndex > 6) {
		logger->err("=== PLAYER ILL DONE DELETING");
		anim->deleteObject = 1;
		obj->visible = 0;
		return 0;
	}

	logger->err("==== Iterate Player Kill DONE ====");
	return 1;
}

void killPlayer(Player* p) {
	logger->enabled = 1;
	logger->err("==== KILL PLAYER ====");
	logger->err("-- Player: %p", p);
	logger->err("-- Player: %s", p->name);

	p->alive = 0;
	p->clipIndex = 0;
	updatePlayerClip(p);
	p->object->collision->enabled = 0;

	AnimParam* anim = customAnim(p->object, 0.3f, 0, iteratePlayerKill);
}