#include "view.h"

SDL_Surface* getScreen() {
	static SDL_Surface* screen = NULL;

	if (screen != NULL) {
		return screen;
	}

	logger->inf("==== Init Window ====");
	SDL_WM_SetCaption("Bomberman", "Bomberman");
	screen = SDL_SetVideoMode(SCREEN_W, SCREEN_H, 0, 0);

	return screen;
}


ListManager* getLayers() {
	static ListManager* layers = NULL;
	
	if (layers != NULL){
		return layers;
	}

	logger->inf("==== Init Layers ====");
	layers = initListMgr();
	return layers;
}

short setObjectLayer(Object* obj, short z) {
	logger->inf("==== Setting Object %s To Layer %d ====", obj->name, z);
	obj->z = z;
	
	ListManager* layers = getLayers();
	Node* layer = getNode(layers, z);

	if (layer == NULL){
		char* layerName = malloc(10);
		sprintf(layerName, "#Layer-%d", z);
		logger->dbg("-- Creating Layer: %s", layerName);
		
		if (!layers->nodeCount){
			logger->dbg("-- Is First Layer");
			layer = addNodeV(layers, layerName, initListMgr(), 0);

			if (layer == NULL){
				logger->err("==== FAIL TO ADD FIRST LAYER: %s ====", layerName);
				return 0;
			}
		}
		else{
			layer = malloc(sizeof(Node));
			layer->id = z;
			layer->name = layerName;
			layer->value = initListMgr();

			short success = listInsertAfter(layers, layer, layers->last->id);
			if (!success){
				logger->err("==== FAIL TO ADD LAYER: %s ====", layerName);
				return 0;
			}
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

			//logger->dbg("Object: %s", objectNode->name);
			//logger->dbg("%d | %d | %d | %d", o->pos.x, o->pos.y, o->pos.x + o->pos.w, o->pos.y + o->pos.h);

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
	logger->inf("==== Printing Object: %s ====", obj->name);
	logger->dbg("-- x=%d | y=%d", obj->pos.x, obj->pos.y);
	
	SDL_Surface* screen = getScreen();

	int tmpx = obj->pos.x;
	int tmpy = obj->pos.y;
    
    if (obj->component != NULL) {
		logger->dbg("-- Casting Surface");
	    SDL_Surface* surf = (SDL_Surface*) obj->component;
		
		logger->dbg("-- Blitting Surface");
		logger->dbg("-- Pos: %d, %d, %d, %d", obj->pos.x, obj->pos.y, obj->pos.w, obj->pos.h);
	    SDL_BlitSurface (surf, NULL, screen, &obj->pos);
	    
	    obj->pos.x = tmpx;
	    obj->pos.y = tmpy;
		logger->dbg("-- X=%d | Y=%d", obj->pos.x, obj->pos.y);
    }
    else{
		logger->war("-- Surface Is NULL");
    }

	logger->dbg("==== Printing Object: %s DONE ====", obj->name);
}

void render() {
	logger->inf("==== Rendering ==== ");
	SDL_Surface* screen = getScreen();
	ListManager* layers = getLayers();

	Node* objNode = NULL;
	Node* childNode = NULL;
	Node* layerNode = NULL;


	while((layerNode = listIterate(layers, layerNode)) != NULL) {
		ListManager* objects = (ListManager*) layerNode->value;
		logger->dbg("-- #%d -- %s", layerNode->id, layerNode->name);

		if (layerNode->id > 1) {
			logger->dbg("Sorting List");
			sortList(objects, &layerSort);
			//return;
		}

		objNode = NULL;
	    while((objNode = listIterate(objects, objNode)) != NULL) {
    		Object* obj = (Object*) objNode->value;
    		if (!obj->visible) {
    			continue;
    		}
    		
			logger->dbg("-- Object: %s", obj->name);
	    	printObject(obj);

	    	if (obj->childs != NULL) {
	    		while((childNode = listIterate(obj->childs, childNode)) != NULL) {
	    			Object* child = (Object*) childNode->value;
					logger->dbg("---- Child: %s", child->name);

	    			if (child->visible) {
	    				printObject(child);
	    			}
	    		}
	    	}
	    }
	}

    SDL_Flip(screen);
    SDL_UpdateRect(screen, 0, 0, 0, 0);

	logger->dbg("==== Rendering DONE ==== ");
}

void buttonUnHover(Object* obj) {
	logger->inf("==== Button Un-Hover %s =====", obj->name);
	if (obj == NULL){
		logger->err("-- Object Is Null");
		return;
	}
	
	Button* btn = (Button*) obj->container;

	char imgPath[30];
	sprintf(imgPath, "asset/%s.png", btn->imgPath);
	logger->dbg("-- image: %s", imgPath);

	SDL_Surface* img = IMG_Load(imgPath);
	if (img == NULL){
		logger->err("==== Fail to get Image: %s ====", imgPath);
		return;
	}

	if (obj->component != NULL) {
		logger->dbg("-- Free Old Surface");
		SDL_FreeSurface(obj->component);
		obj->component = NULL;
	}
	
	obj->component = img;
	logger->dbg("==== Button Un-Hover %s DONE =====", obj->name);
}

void buttonHover(Object* obj) {
	logger->inf("==== Button Hover %s =====", obj->name);
	if (obj == NULL){
		logger->err("Object Is Null");
		return;
	}

	logger->dbg("-- %s", obj->name);
	Button* btn = (Button*)obj->container;

	char imgPath[35];
	sprintf(imgPath, "asset/%s-hover.png", btn->imgPath);
	logger->dbg("-- image: %s", imgPath);
	
	SDL_Surface* img = IMG_Load(imgPath);
	if (img == NULL){
		logger->err("==== Fail to get Image: %s ====", imgPath);
		return;
	}

	if (obj->component != NULL) {
		logger->dbg("-- Free Old Surface");
		SDL_FreeSurface(obj->component);
		obj->component = NULL;
	}

	obj->component = img;
	logger->dbg("==== Button Hover %s DONE =====", obj->name);
}


void deleteBtn(Button* btn) {
	//logger->inf("Deleting Button: %s", btn->name);
	deleteObject(btn->txtObj);
	deleteObject(btn->imgObj);
}

Object** getHovered() {
	static Object* obj = NULL;
	return &obj;
}