#include "map.h"

ListManager* getBlockList() {
	static ListManager* blocks = NULL;
	if(blocks != NULL) {
		return blocks;
	}

	blocks = initListMgr();
	return blocks;
}

void generateWalls() {
	Game* game = getGame();
	enableLogger(DBG_MAP);

	logger->inf("==== PLACING WALLS ====");
	int x;
	int y;

	char name[35];
	Object* wall = NULL;

	SDL_Rect hit = {0,0, CELL_SIZE, CELL_SIZE};
	SDL_Rect pos = {0,0, CELL_SIZE, CELL_SIZE};

	int colideWith = COL_PLAYER | COL_BOMB;

	for (y = 1; y < MAP_H; ++y) {
		if ( y % 2) {
			pos.y = MAP_Y + (y * CELL_SIZE);

			for (x = 1; x < MAP_W; ++x) {
				if (x % 2) {

					snprintf(name, 35, "wall_%d-%d", x, y);
					logger->dbg("Placing Wall: %s", name);

					pos.x = MAP_X + (x * CELL_SIZE);

					logger->dbg("pos: x: %d, y: %d", pos.x, pos.y);

					wall = addSimpleObject(name, NULL, &pos, 2);
					setHitBox(wall, hit, 1, COL_WALL);
					wall->collision->colFlags = colideWith;
					wall->visible = 0;
				}
			}
		}
	}

	int color = 0;
	if (game->flags & DBG_HIT) {
		logger->dbg("FILL COLOR: %d", 0x03B3BEE);
		color = 0x03B3BEE;
		//assert(0);
	}

	hit.x = 0;
	hit.y = 0;

	// PLACING MAP END LEFT //
	pos.x = MAP_X - 25;
	pos.y = MAP_Y - 25;

	pos.w = 25;
	pos.h = (MAP_H + 1) * CELL_SIZE;

	wall = addSimpleObject("Map-End-LEFT", NULL, &pos, 2);
	wall->visible = game->flags & DBG_HIT;
	wall->color = color;

	hit.w = 25;
	hit.h = (MAP_H + 1) * CELL_SIZE;
	setHitBox(wall, hit, 1, COL_WALL);
	wall->collision->colFlags = colideWith;

	// PLACING MAP END TOP //
	pos.x = MAP_X - 25;
	pos.y = MAP_Y - 30;

	pos.h = 25;
	pos.w = (MAP_W + 1) * CELL_SIZE;

	wall = addSimpleObject("Map-End-Top", NULL, &pos, 2);
	wall->visible = game->flags & DBG_HIT;
	wall->color = color;

	hit.h = 25;
	hit.w = (MAP_W + 1) * CELL_SIZE;
	setHitBox(wall, hit, 1, COL_WALL);
	wall->collision->colFlags = colideWith;

	// PLACING MAP END BOTTOM //
	pos.x = MAP_X - 25;
	pos.y = (MAP_H * CELL_SIZE) + MAP_Y;

	pos.h = 25;
	pos.w = (MAP_W + 1) * CELL_SIZE;

	wall = addSimpleObject("Map-End-BOTTOM", NULL, &pos, 2);
	wall->visible = game->flags & DBG_HIT;
	wall->color = color;

	hit.h = 25;
	hit.w = (MAP_W + 1) * CELL_SIZE;
	setHitBox(wall, hit, 1, COL_WALL);
	wall->collision->colFlags = colideWith;

	// PLACING MAP END RIGHT //
	pos.y = MAP_Y - 25;
	pos.x = ((MAP_W + 1) * CELL_SIZE) - 10;

	pos.w = 25;
	pos.h = (MAP_H + 1) * CELL_SIZE;

	wall = addSimpleObject("Map-End-RIGHT", NULL, &pos, 2);
	wall->visible = game->flags & DBG_HIT;
	wall->color = color;

	hit.w = 25;
	hit.h = (MAP_H + 1) * CELL_SIZE;
	setHitBox(wall, hit, 1, COL_WALL);
	wall->collision->colFlags = colideWith;

	generateBlocks();
}

void generateBlocks() {
	Game* game = getGame();

	Block* block = NULL;
	Object* blockObj = NULL;
	AssetMgr* ast = getAssets();
	SDL_Surface* img = ast->getImg("objects");

	int i = 0;
	char name[100];

	int x = 0;
	int y = 0;

	ListManager* blockList = getBlockList();
	SDL_Rect hit = {0, 0, CELL_SIZE, CELL_SIZE};
	SDL_Rect pos = {0, 0, CELL_SIZE, CELL_SIZE};

	for (y = 0; y < MAP_H; ++y) {
		logger->enabled = game->flags & DBG_MAP;

		for (x = 0; x < MAP_W; ++x) {
			logger->enabled = game->flags & DBG_MAP;

			if ((y <= 1 || y >= MAP_H -2) && (x <= 1 || x >= MAP_W -2)) {
			 	logger->dbg("Skiping\n-- x: %d\n-- y: %d\n==========\n", x, y);
				continue;
			}

			if ((y % 2) && (x % 2)) {
			 	logger->dbg("Skiping\n-- x: %d\n-- y: %d\n==========\n", x, y);
				continue;
			}

			snprintf(name, 100, "block-%d", i++);
			logger->dbg("-- Placing: %s", name);

			block = malloc(sizeof(Block));
			block->bonusType = BNS_NONE;
			block->state = 0;
			block->destroyed = 0;

			block->clip.x = 0;
			block->clip.y = 2 * BONUS_SIZE;

			block->clip.w = CELL_SIZE;
			block->clip.h = CELL_SIZE;

			pos.y = MAP_Y + (y * CELL_SIZE);
			pos.x = MAP_X + (x * CELL_SIZE);

			blockObj = addSimpleObject(name, img, &pos, 2);
			setHitBox(blockObj, hit, 1, COL_WALL);
			block->obj = blockObj;

			Node* blockNode = addNodeV(blockList, name, block, 0);
			block->id = blockNode->id;

			blockObj->visible = 1;
			blockObj->container = block;
			blockObj->clip = &block->clip;

			blockObj->containerType = BLOCK;

			blockObj->collision->flag = COL_BLOCK;
			blockObj->collision->colFlags = COL_PLAYER | COL_BOMB;
		}
	}
}

void iterateBlock(AnimParam* anim) {
	Object* obj = (Object*) anim->obj;
	Block* block = (Block*) obj->container;

	enableLogger(DBG_MAP);

	block->state++;
	logger->dbg("-- Block state: %d", block->state);

	SDL_Rect animClip = {0, 0, CELL_SIZE, CELL_SIZE};

	animClip.y = 2 * BONUS_SIZE;
	animClip.x = (block->state) * CELL_SIZE;

	block->clip.y = BONUS_SIZE * 2;
	block->clip.x = (block->state) * CELL_SIZE;

	//logger->dbg("Cur Clip: x: %d, y: %d", block->clip.x, block->clip.y);


	if (block->state < 3) {
		logger->dbg("-- Block iterate");
		AnimParam* anim = spriteAnim(block->obj, animClip, 0.1f, 0, 0);
		anim->callBack = iterateBlock;
	}
	else{
		logger->dbg("-- Prepare To Delete");
		anim->deleteObject = 1;
	}
}

void breakBlock(Block* block) {
	if (block->destroyed) {
		return;
	}

	block->state = 0;
	block->destroyed = 1;

	SDL_Rect animClip = {0, 0, CELL_SIZE, CELL_SIZE};

	animClip.x = 0;
	animClip.y = 2 * BONUS_SIZE;
	//logger->dbg("Break Clip: x: %d, y: %d", animClip.x, animClip.y);

	AnimParam* anim = spriteAnim(block->obj, animClip, 0.1f, 0, 0);
	anim->callBack = iterateBlock;
}
