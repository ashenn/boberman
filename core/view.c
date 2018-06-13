#include "view.h"

SDL_Surface* getScreen() {
	static SDL_Surface* screen = NULL;

	if (screen != NULL) {
		return screen;
	}

	Game* game = getGame();
	logger->enabled = game->flags & DBG_VIEW;

	logger->inf("==== Init Window ====");
	SDL_WM_SetCaption("Bomberman", "Bomberman");
	screen = SDL_SetVideoMode(SCREEN_W, SCREEN_H, 0, 0);

	return screen;
}

void clearScreen() {
	Game* game = getGame();
	logger->enabled = game->flags & DBG_VIEW;


	logger->dbg("==== Clearing Screen ==== ");

	SDL_Surface* screen = getScreen();
	SDL_FillRect(screen, NULL, 0x000000);

    SDL_Flip(screen);
    SDL_UpdateRect(screen, 0, 0, 0, 0);
}

ListManager* getLayers() {
	static ListManager* layers = NULL;
	
	if (layers != NULL){
		return layers;
	}

	Game* game = getGame();
	logger->enabled = game->flags & DBG_VIEW;


	logger->inf("==== Init Layers ====");
	layers = initListMgr();
	return layers;
}

short setObjectLayer(Object* obj, short z) {
	Game* game = getGame();
	logger->enabled = game->flags & DBG_VIEW;


	logger->inf("==== Setting Object %s To Layer %d ====", obj->name, z);
	obj->z = z;
	
	ListManager* layers = getLayers();
	Node* layer = getNode(layers, z);

	if (layer == NULL){
		char layerName[12];
		snprintf(layerName, 12, "#Layer-%d", z);
		
		logger->dbg("-- Creating Layer: %s", layerName);
		
		layer = addNodeV(layers, layerName, initListMgr(), 0);

		if (layer == NULL){
			logger->err("==== FAIL TO ADD LAYER: %s ====", layerName);
			return 0;
		}
		
	}

	ListManager* layerObjs = (ListManager*)layer->value;
	
	logger->dbg("-- Adding Object To Loayer");
	Node* objNode = addNodeV(layerObjs, obj->name, obj, 0);
	
	logger->dbg("-- nodeID: %d", objNode->id);
	objNode->id = obj->id;
	
	logger->dbg("==== Object %s Added To Layer: %s ====", obj->name, layer->name);

	return 1;
}

short isHovered(Object* obj, int x, int y) {
	return 	
		x >= obj->pos.x && 
		x <= obj->pos.x + obj->pos.w
		&&
		y >= obj->pos.y && 
		y <= obj->pos.y + obj->pos.h
	;
}

Object* getClicked(int x, int y, short hover) {
	Game* game = getGame();
	logger->enabled = game->flags & DBG_MOUSE;


	if (!hover) {
		logger->inf("==== Find Hover ====");
	}
	else {
		logger->inf("==== Find Click ====");
	}

	logger->dbg("-- pos: x: %d | y: %d", x, y);

	Object* o = NULL;
	Node* layerNode = NULL;
	Node* objectNode = NULL;
	ListManager* objects = NULL;
	ListManager* layers = getLayers();

	while((layerNode = listRevIterate(layers, layerNode)) != NULL) {
		logger->dbg("-- Layer: %d", layerNode->id);

		if (layerNode->id == 1){
			break;
		}

		objects = (ListManager*) layerNode->value;

		while((objectNode = listRevIterate(objects, objectNode)) != NULL) {
			o = (Object*) objectNode->value;
			logger->dbg("---- Object: %s", o->name);

			if ((o->click == NULL && !hover) || o->hover == NULL) {
				continue;
			}

			if (isHovered(o, x, y)) {
				logger->inf("==== Object Found: %s ====", o->name);
				return o;
			}

		}
	}

	logger->inf("==== No Object Found ====");
	return NULL;
}

void printObject(Object* obj) {
	Game* game = getGame();
	logger->enabled = game->flags & DBG_VIEW;


	logger->inf("==== Printing Object: %s ====", obj->name);
	logger->dbg("-- x=%d | y=%d", obj->pos.x, obj->pos.y);
	
	SDL_Surface* screen = getScreen();

	int tmpx = obj->pos.x;
	int tmpy = obj->pos.y;
    
    if (obj->component != NULL) {
		logger->dbg("-- Casting Surface");
	    SDL_Surface* surf = (SDL_Surface*) obj->component;
		
		logger->dbg("-- Blitting Surface");
		logger->dbg("-- Pos: x: %d, y: %d, w: %d, h: %d", obj->pos.x, obj->pos.y, obj->pos.w, obj->pos.h);

		if (obj->clip != NULL) {
			logger->dbg("-- Clip: x: %d, y: %d, w: %d, h: %d", obj->clip->x, obj->clip->y, obj->clip->w, obj->clip->h);
		}
		
    	SDL_BlitSurface (surf, obj->clip, screen, &obj->pos);
	    
	    obj->pos.x = tmpx;
	    obj->pos.y = tmpy;
		logger->dbg("-- X=%d | Y=%d", obj->pos.x, obj->pos.y);
    }
    else if(obj->color) {
    	logger->err("FILL COLOR: ");
    	SDL_FillRect(screen, &obj->pos, obj->color);
    }
    else if(obj->childs == NULL || !obj->childs->nodeCount){
		logger->war("-- Surface Is NULL for object: %s", obj->name);
    }

	logger->dbg("==== Printing Object: %s DONE ====", obj->name);
}

void renderObject(Object* obj) {
	Game* game = getGame();
	logger->enabled = game->flags & DBG_VIEW;

	logger->dbg("-- Object: %s", obj->name);
	//pthread_cond_wait (&obj->threadCond, &obj->threadMutex);
	printObject(obj);

	if (obj->childs != NULL) {
		Node* childNode = NULL;
		
		while((childNode = listIterate(obj->childs, childNode)) != NULL) {
			Object* child = (Object*) childNode->value;
			logger->dbg("---- Child: %s", child->name);

			if (child->visible) {
				renderObject(child);
			}
		}
	}
}

void renderObjectList() {
	Node* objNode = NULL;
	Node* layerNode = NULL;	
	ListManager* layers = getLayers();
	
	if (!layers->nodeCount) {
		return;
	}

	while((layerNode = listIterate(layers, layerNode)) != NULL) {
		ListManager* objects = (ListManager*) layerNode->value;
		logger->dbg("-- #%d -- %s", layerNode->id, layerNode->name);

		if (!objects->nodeCount) {
			continue;
		}
		else if (layerNode->id > 1) {
			//logger->err("===== Sorting List =====");
			sortList(objects, &layerSort);
		}

		objNode = NULL;
	    while((objNode = listIterate(objects, objNode)) != NULL) {
    		Object* obj = (Object*) objNode->value;
    		if (!obj->visible) {
    			continue;
    		}
    		
			renderObject(obj);
	    }
	}
}

void* render(void* arg) {
	Game* game = getGame();
	SDL_Surface* screen = getScreen();

	int t = 0;

	int add = 0;
	int now = 0;
    int nextFrame = 0;

	//logger->err("RENDER: Ask-Lock");
	lock(DBG_VIEW);
	//logger->err("RENDER: Lock");

	now = SDL_GetTicks();
	add = (int)(1000 / (float)FPS);
    nextFrame = now + add;

    while(game->status != GAME_QUIT) {

	    
	    //logger->err("Start: %d", now);
	    //logger->err("Add: %d", add);
	    //logger->err("TOTAL: %d", nextFrame);
		
    	t = SDL_GetTicks();
		//logger->err("==== Rendering ==== ");
		//logger->inf("==== Animating ==== ");
		animate();
		renderObjectList();
		//pthread_cond_signal(&game->cond);

	    SDL_Flip(screen);
	    SDL_UpdateRect(screen, 0, 0, 0, 0);

	    //logger->err("RENDER: Un-Lock");
	    unlock(DBG_VIEW);
		//logger->err("==== Rendering END %d ====", SDL_GetTicks() - t);
		

	    tickWait(nextFrame);

    	now = SDL_GetTicks();
    	add = (int)(1000 / (float)FPS);
        nextFrame = now + add;

	    //logger->err("RENDER: Ask-Lock");
	    lock(DBG_VIEW);
	    //logger->err("RENDER: Lock");
		//logger->dbg("==== Rendering DONE ==== ");
    }

    pthread_exit(NULL);
}

void buttonUnHover(Object* obj) {
	Game* game = getGame();
	logger->enabled = game->flags & DBG_MOUSE;

	logger->inf("==== Button Un-Hover %s =====", obj->name);
	if (obj == NULL){
		logger->err("-- Object Is Null");
		return;
	}
	
	Button* btn = (Button*) obj->container;

	AssetMgr* ast = getAssets();

	char imgPath[30];
	sprintf(imgPath, "asset/%s.png", btn->imgPath);
	logger->dbg("-- image: %s", imgPath);

	SDL_Surface* img = ast->getImg(btn->imgPath);
	if (img == NULL){
		logger->err("==== Fail to get Image: %s ====", imgPath);
		return;
	}
	
	obj->component = img;
	logger->dbg("==== Button Un-Hover %s DONE =====", obj->name);
}

void buttonHover(Object* obj) {
	Game* game = getGame();
	logger->enabled = game->flags & DBG_MOUSE;


	logger->inf("==== Button Hover %s =====", obj->name);
	if (obj == NULL){
		logger->err("Object Is Null");
		return;
	}

	logger->dbg("-- %s", obj->name);
	Button* btn = (Button*)obj->container;

	AssetMgr* ast = getAssets();

	char imgPath[35];
	sprintf(imgPath, "%s-hover", btn->imgPath);
	logger->dbg("-- image: %s", imgPath);
	
	SDL_Surface* img = ast->getImg(imgPath);
	if (img == NULL){
		logger->err("==== Fail to get Image: %s ====", imgPath);
		return;
	}

	obj->component = img;
	logger->dbg("==== Button Hover %s DONE =====", obj->name);
}


void deleteBtn(Button* btn) {
	deleteObject(btn->txtObj);
	deleteObject(btn->imgObj);
}

Object** getHovered() {
	static Object* obj = NULL;
	return &obj;
}

void showGame() {
	
}