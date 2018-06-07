#include "map.h"

void generateWalls() {
	logger->inf("==== PLACING WALLS ====");
	int x;
	int y;
	
	char name[35];
	Object* wall = NULL;

	SDL_Rect hit = {0,0, CELL_SIZE, CELL_SIZE};
	SDL_Rect pos = {0,0, CELL_SIZE, CELL_SIZE};
	
	for (y = 1; y < MAP_H; ++y) {
		if ( y % 2) {
			pos.y = MAP_Y + (y * CELL_SIZE);
			
			for (x = 1; x < MAP_W; ++x) {
				if (x % 2) {

					snprintf(name, 35, "wall_%d-%d", x, y);
					logger->err("Placing Wall: %s", name);

					pos.x = MAP_X + (x * CELL_SIZE);
					
					logger->err("pos: x: %d, y: %d", pos.x, pos.y);
					
					wall = addSimpleObject(name, NULL, &pos, 2);
					setHitBox(wall, hit, 1);
				}
			}
		}
	}

	Game* game = getGame();

	int color = 0;
	if (game->flags & DBG_HIT) {
		color = 0x03B3BEE;
	}

	hit.x = 0;
	hit.y = 0;

	// PLACING MAP END LEFT //
	pos.x = MAP_X - 25;
	pos.y = MAP_Y - 25;
	
	pos.w = 25;
	pos.h = (MAP_H + 1) * CELL_SIZE;

	wall = addSimpleObject("Map-End-LEFT", NULL, &pos, 2);
	wall->color = color;

	hit.w = 25;
	hit.h = (MAP_H + 1) * CELL_SIZE;
	setHitBox(wall, hit, 1);

	// PLACING MAP END TOP //
	pos.x = MAP_X - 25;
	pos.y = MAP_Y - 30;
	
	pos.h = 25;
	pos.w = (MAP_W + 1) * CELL_SIZE;

	wall = addSimpleObject("Map-End-Top", NULL, &pos, 2);
	wall->color = color;

	hit.h = 25;
	hit.w = (MAP_W + 1) * CELL_SIZE;
	setHitBox(wall, hit, 1);

	// PLACING MAP END BOTTOM //
	pos.x = MAP_X - 25;
	pos.y = (MAP_H * CELL_SIZE) + MAP_Y;
	
	pos.h = 25;
	pos.w = (MAP_W + 1) * CELL_SIZE;

	wall = addSimpleObject("Map-End-BOTTOM", NULL, &pos, 2);
	wall->color = color;

	hit.h = 25;
	hit.w = (MAP_W + 1) * CELL_SIZE;
	setHitBox(wall, hit, 1);

	// PLACING MAP END RIGHT //
	pos.y = MAP_Y - 25;
	pos.x = ((MAP_W + 1) * CELL_SIZE) - 10;
	
	pos.w = 25;
	pos.h = (MAP_H + 1) * CELL_SIZE;

	wall = addSimpleObject("Map-End-RIGHT", NULL, &pos, 2);
	wall->color = color;

	hit.w = 25;
	hit.h = (MAP_H + 1) * CELL_SIZE;
	setHitBox(wall, hit, 1);

	generateBlocks();
}

void generateBlocks() {
	Block* block = NULL;
	Object* blockObj = NULL;
	AssetMgr* ast = getAssets();
	SDL_Surface* img = ast->getImg("objects");

	int i = 0;
	char name[100];
	
	int x = 0;
	int y = 0;

	SDL_Rect hit = {0, 0, CELL_SIZE, CELL_SIZE};
	SDL_Rect pos = {0, 0, CELL_SIZE, CELL_SIZE};



	for (y = 0; y < 9; ++y) {
		logger->enabled = 1;
		logger->err("Y: %d", y);

		for (x = 2; x < 11; ++x) {
			logger->enabled = 1;
			logger->err("X: %d", x);
			
			if (y && (y % 2) && (x % 2)) {
				logger->err("Skiping");
				continue;
			}

			snprintf(name, 100, "block-%d", i++);
			logger->err("Placing: %s", name);
		
			block = malloc(sizeof(Block));
			block->state = 0;
			block->destroyed = 0;

			block->clip.y = CELL_SIZE;
			block->clip.w = CELL_SIZE;
			block->clip.h = CELL_SIZE;
			block->clip.x = 2 * CELL_SIZE;

			pos.y = MAP_Y + (y * CELL_SIZE);
			pos.x = MAP_X + (x * CELL_SIZE);
			
			blockObj = addSimpleObject(name, img, &pos, 2);
			setHitBox(blockObj, hit, 1);
			block->obj = blockObj;

			blockObj->container = block;
			blockObj->clip = &block->clip;
			blockObj->containerType = BLOCK;
		}
	}
}

void iterateBlock(AnimParam* anim) {
	Object* obj = (Object*) anim->obj;
	Block* block = (Block*) obj->container;

	logger->enabled = 1;
	block->state++;
	logger->err("BLOCK STATE: %d", block->state);
	
	SDL_Rect animClip = {0, CELL_SIZE, CELL_SIZE, CELL_SIZE};
	animClip.x = (block->state + 1) * CELL_SIZE;

	block->clip.y = CELL_SIZE;
	block->clip.x = (block->state + 2) * CELL_SIZE;

	if (block->state < 3) {
		logger->err("ITERATE");
		AnimParam* anim = spriteAnim(block->obj, animClip, 0.3f, 0, 0);
		anim->callBack = iterateBlock;
	}
	else{
		logger->err("Prepare To Delete");
		anim->deleteObject = 1;
	}
}

void breakBlock(Block* block) {
	if (block->destroyed) {
		return;
	}

	block->state++;
	block->destroyed = 1;
	
	SDL_Rect animClip = {0, CELL_SIZE, CELL_SIZE, CELL_SIZE};
	animClip.x = (block->state + 1) * CELL_SIZE;

	AnimParam* anim = spriteAnim(block->obj, animClip, 0.3f, 0, 0);
	anim->callBack = iterateBlock;
}