#include "view.h"

SDL_Surface* getScreen() {
	static SDL_Surface* screen = NULL;

	if (screen != NULL) {
		return screen;
	}

	SDL_WM_SetCaption("Bomberman", "Bomberman");
	screen = SDL_SetVideoMode(SCREEN_W, SCREEN_H, 0, 0);

	return screen;
}


ListManager* getLayers() {
	static ListManager* layers = NULL;
	
	if (layers != NULL){
		return layers;
	}

	layers = initListMgr();
	return layers;
}

short setObjectLayer(Object* obj, short z) {
	obj->z = z;
	ListManager* layers = getLayers();
	Node* layer = getNode(layers, z);

	if (layer == NULL){
		char* layerName = malloc(10);
		sprintf(layerName, "#Layer-%d", z);
		
		if (!layers->nodeCount){
			layer = addNodeV(layers, layerName, initListMgr(), 0);

			if (layer == NULL){
				return 0;
			}
		}
		else{
			layer = malloc(sizeof(Node));
			layer->id = z;
			layer->name = layerName;
			layer->value = initListMgr();

			short success = listInsertAfter(layers, layer, layers->last->id);
			if (layer == NULL){
				return 0;
			}
		}
	}

	ListManager* layerObjs = (ListManager*)layer->value;
	Node* objNode = addNodeV(layerObjs, obj->name, obj, 0);
	objNode->id = obj->id;

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
	Object* o = NULL;
	Node* layerNode = NULL;
	Node* objectNode = NULL;
	ListManager* objects = NULL;
	ListManager* layers = getLayers();

	while((layerNode = listRevIterate(layers, layerNode)) != NULL) {
		//logger->dbg("LAYER: %d", layerNode->id);
		if (layerNode->id == 1){
			break;
		}
		objects = (ListManager*) layerNode->value;

		while((objectNode = listRevIterate(objects, objectNode)) != NULL) {
			o = (Object*) objectNode->value;
			if ((o->click == NULL && !hover) || o->hover == NULL) {
				continue;
			}

			//logger->dbg("Object: %s", objectNode->name);
			//logger->dbg("%d | %d | %d | %d", o->pos.x, o->pos.y, o->pos.x + o->pos.w, o->pos.y + o->pos.h);

			if (isHovered(o, x, y)) {
				return o;
			}

		}
	}

	return NULL;
}

void printObject(Object* obj) {
	// logger->war("-- %s", obj->name);
	// logger->war("-- X=%d | Y=%d", obj->pos.x, obj->pos.y);
	
	SDL_Surface* screen = getScreen();

	int tmpx = obj->pos.x;
	int tmpy = obj->pos.y;
    
    SDL_Surface* surf = (SDL_Surface*) obj->component;
    SDL_BlitSurface (surf, NULL, screen, &obj->pos);

    obj->pos.x = tmpx;
    obj->pos.y = tmpy;
	// logger->war("-- X=%d | Y=%d", obj->pos.x, obj->pos.y);
}

void render() {
	logger->war("-- Rendering-- ");
	SDL_Surface* screen = getScreen();
	ListManager* layers = getLayers();

	Node* objNode = NULL;
	Node* layerNode = NULL;


	while((layerNode = listIterate(layers, layerNode)) != NULL) {
		ListManager* objects = (ListManager*) layerNode->value;
		logger->war("-- %s", layerNode->name);

		objNode = NULL;
	    while((objNode = listIterate(objects, objNode)) != NULL) {
    		Object* obj = (Object*) objNode->value;
    		if (!obj->visible) {
    			return;
    		}
			logger->war("-- Object: %s", obj->name);
	    	printObject(obj);

	    	if (obj->childObj != NULL) {
	    		printObject(obj->childObj);
	    	}
	    }
	}

    SDL_Flip(screen);
    SDL_UpdateRect(screen, 0, 0, 0, 0);
	// logger->war("-- Rendering DONE-- ");
}

void buttonUnHover() {
	Object** obj = getHovered();
	Button* btn = (Button*)(*obj)->container;
	
	if (*obj == NULL){
		return;
	}

	char imgPath[30];
	sprintf(imgPath, "asset/%s.png", btn->imgPath);
	//logger->dbg("IMG Path: %s", imgPath);
	
	SDL_Surface* img = IMG_Load(imgPath);
	if (img == NULL){
		//logger->err("Fail to get Image: %s", imgPath);
		return;
	}

	SDL_FreeSurface((*obj)->component);
	(*obj)->component = img;
}

void buttonHover(Object* obj) {
	Button* btn = (Button*)obj->container;

	char imgPath[35];
	sprintf(imgPath, "asset/%s-hover.png", btn->imgPath);
	//logger->dbg("IMG Path: %s", imgPath);
	
	SDL_Surface* img = IMG_Load(imgPath);
	if (img == NULL){
		//logger->err("Fail to get Image: %s", imgPath);
		return;
	}

	SDL_FreeSurface(obj->component);
	obj->component = img;
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