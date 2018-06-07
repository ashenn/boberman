#include "animation.h"
#include <stdio.h>
#include <math.h>

AnimParam* initAnimParam(Object* obj, float time, float delay, void* fnc) {
	AnimParam* param = malloc(sizeof(AnimParam));

	param->obj = obj;
	param->fnc = fnc;
	param->time = time;
	param->boolean = 0;
	param->custFnc = NULL;
	param->callBack = NULL;
	param->deleteObject = 0;
	param->delay = delay * FPS;
	param->frames = time * FPS;

	return param;
}

Animator* getAnimator() {
	static Animator* animator = NULL;

	if (animator != NULL) {
		return animator;
	}

	logger->inf("==== Init Animator ====");
	animator = malloc(sizeof(Animator));
	animator->objects = initListMgr();
	
	return animator;
}

AnimDistance animDistanceByFrame(int dist, float time) {
	logger->inf("==== Calculation Animation Distance Per Frame ====");
	logger->dbg("-- dist: %dpx\n--time: %fs", dist, time);

	int rest;
	int frames;
	int perFrame;
	AnimDistance animDist;

	if ((FPS * time) < 1){
		frames = 1;
	}
	else {
		frames = (FPS * time);
	}
	logger->dbg("-- total frames: %d", frames);

	perFrame = (dist / frames);
	rest = (dist % frames);

	animDist.perFrame = perFrame + (rest / frames);
	animDist.rest = rest % frames;

	logger->dbg("-- dist per frame: %d\n--rest: %d", animDist.perFrame, animDist.rest);
	logger->dbg("==== Calculation DONE ====");

	return animDist;
}

void animSetPostion(Object* obj, int x, int y) {
	logger->inf("==== Set Object Position %s =>  x: %d  | y: %d", obj->name, x, y);
	obj->pos.x = x;
	obj->pos.y = y;
}

void* animMoveTo(AnimParam* param) {
	logger->inf("==== Anim Move %s ====", param->obj->name);
	Object* obj = param->obj;

	logger->dbg(
		"Animation: Moving ID: #%d Obj: %s\nX = %d\nY = %d",
		param->id,
		obj->name,
		obj->pos.x,
		obj->pos.y
	);
	
	int xMove = param->xDist.perFrame;
	int yMove = param->yDist.perFrame;

	if (param->xDist.rest != 0) {
		logger->dbg("-- Moving X Rest = %d", param->xDist.rest);
		if (param->xDist.rest > 0) {
			xMove++;
			param->xDist.rest--;
		}
		else{
			xMove--;
			param->xDist.rest++;
		}

	}

	if (param->yDist.rest != 0) {
		logger->dbg("-- Moving Y Rest = %d", param->yDist.rest);
		if (param->yDist.rest > 0) {
			yMove++;
			param->yDist.rest--;
		}
		else{
			yMove--;
			param->yDist.rest++;
		}

	}
	
	logger->dbg("-- Moving Coords: X = %d | Y = %d", xMove, yMove);

	animSetPostion(
		obj, 
		obj->pos.x + xMove,
		obj->pos.y + yMove
	);

	if (obj->childs != NULL) {
		Node* childNode = NULL;
		logger->dbg("-- Moving Childs: %d", obj->childs->nodeCount);

		while((childNode = listIterate(obj->childs, childNode)) != NULL) {
			Object* child = (Object*) childNode->value;
			logger->dbg("-- Child: %s\nvisible: %d", child->name, child->visible);

			if (child->visible) {
				animSetPostion(
					child, 
					child->pos.x + xMove,
					child->pos.y + yMove
				);
			}
		}
	}
}

void animRemoveObject(Object* obj) {
	logger->inf("==== Removing Anim Object: %s ====", obj->name);
	
	Animator* animator = getAnimator();
	
	Node* objNode = getNodeByName(animator->objects, obj->name);
	
	if (objNode == NULL) {
		logger->dbg("==== Removing Anim Not Found ====");
		return;
	}

	logger->dbg("--clearing queue");
	deleteList(objNode->value);
	deleteNode(animator->objects, objNode->id);

	logger->dbg("==== Removing Anim %s DONE ====", obj->name);
}

AnimParam* animAddObject(Object* obj, AnimParam* param) {
	logger->inf("==== Adding Anim Object: %s ====", obj->name);
	Animator* animator = getAnimator();

	Node* objNode = getNodeByName(animator->objects, obj->name);
	if (objNode == NULL) {
		logger->dbg("-- Animation: Adding New Object: %s", obj->name);
		objNode = addNodeV(animator->objects, obj->name, initListMgr(), 0);
		
		logger->dbg("-- nodeID: %d", objNode->id);
	}

	Node* paramNode = addNodeV(objNode->value, obj->name, param, 1);
	
	param->id = paramNode->id;
	
	logger->dbg("==== Animation: Added %s #%d ====", param->obj->name, paramNode->id);

	return param;
}

AnimParam* moveTo(Object* obj, int x, int y, float time, float delay) {
	if (!canMoveTo(obj, x, y)) {
		return NULL;
	}

	logger->inf("==== Animation: Moving: %s to %d | %d (%fs) ====", obj->name, x, y, time);
	SDL_Rect target;
	target.x = x;
	target.y = y;

	vector vec = getVector(obj->pos, target);
	logger->dbg("-- Vector: %d | %d", vec.x, vec.y);

	AnimParam* param = initAnimParam(obj, time, delay, animMoveTo);
	
	logger->dbg("-- frames: %d", param->frames);
	logger->dbg("-- time: %f", time);

	int xDist = getDistance(vec.x, target.x);
	int yDist = getDistance(vec.y, target.y);

	param->xDist = animDistanceByFrame(vec.x, time);
	logger->dbg("-- xTotDist: %d", param->xDist);
	
	param->yDist = animDistanceByFrame(vec.y, time);
	logger->dbg("-- yTotDist: %d", param->yDist);

	param->fnc = (void*)animMoveTo;
	logger->dbg("-- Param Ready");

	animAddObject(obj, param);
	
	logger->dbg("==== Animation Added ====");
	return param;
}

void* spriteChange(AnimParam* param) {
	Object* obj = param->obj;
	Bomb* bomb = NULL;
	SDL_Rect diff = {0,0,0,0};

	switch(obj->containerType) {
		case BOMB:
			bomb = (Bomb*) obj->container;
			diff.x = param->target.x - bomb->clip.x;
			diff.y = param->target.y - bomb->clip.y;
			diff.w = param->target.w - bomb->clip.w;
			diff.h = param->target.h - bomb->clip.h;
			
			bomb->clip = param->target;
			break;
	}
	
	if (!param->boolean){
		return NULL;
	}

	Node* n = NULL;
	while(n = listIterate(obj->childs, n)) {
	    Object* child = (Object*) n->value;
	    child->clip->x += diff.x;
	    child->clip->y += diff.y;
	    child->clip->w += diff.w;
	    child->clip->h += diff.h;
	}
}

AnimParam* spriteAnim(Object* obj, SDL_Rect clip, float time, float delay, short applyToChilds) {
	AnimParam* param = initAnimParam(obj, time, delay, spriteChange);

	param->target = clip;
	param->boolean = applyToChilds;

	animAddObject(obj, param);

	return param;
}

AnimParam* customAnim(Object* obj, float loopTime, float delay, short (*fnc) (AnimParam*)) {
	logger->err("==== Adding Custom Anim ====");
	logger->err("-- Object: %s", obj->name);
	AnimParam* param = initAnimParam(obj, loopTime, delay, NULL);

	param->custFnc = fnc;	
	animAddObject(obj, param);

	logger->err("==== Custom Anim Added ====");
	return param;
}

void animate() {
	logger->inf("==== Animating Objects ====");
	Node* objNode = NULL; 
	Node* paramNode = NULL; 
	Animator* animator = getAnimator(); 
	ListManager* objList = NULL;

	Object* objDel = NULL;

	while((objNode = listIterate(animator->objects, objNode)) != NULL) {
		objDel = NULL;
	    objList = (ListManager*) objNode->value;
		logger->dbg("-- Object: %s", objNode->name);

    	paramNode = NULL;		
	    while((paramNode = listIterate(objList, paramNode)) != NULL) {
	    	AnimParam* param = (AnimParam*) paramNode->value;
			logger->dbg("-- Animation: #%d %s", paramNode->id, param->obj->name);

	    	if (param->delay > 0) {
				logger->dbg("-- delayed");
	    		param->delay--;
	    		break;
	    	}

	    	if (param->fnc != NULL) {
				logger->dbg("-- calling Anim Function");
	    		param->fnc(param);
	    	}

	    	param->frames--;
			logger->dbg("-- Frames Left: %d", param->frames);

	    	if (param->frames <= 0){
	    		if (param->custFnc != NULL) {
	    			short loop = param->custFnc(param);
	    			if (loop) {
	    				param->frames = param->time * FPS;
	    				break;
	    			}
	    		}

	    		if (param->callBack != NULL){
					logger->dbg("-- Animation: #%d CallBack", paramNode->id);
	    			param->callBack(param);
	    		}
	    		
	    		if (param->deleteObject) {
	    			objDel = param->obj;
	    		}

				logger->dbg("-- Animation: #%d END", paramNode->id);
	    		deleteNode(objList, param->id);
	    	}

	    	break;
	    }

		if (!objList->nodeCount) {
			logger->dbg("-- Animation: No More Anim For Object: %s", objNode->name);

			deleteList(objList);
			
			int id = objNode->id;
			objNode = objNode->prev;
			deleteNode(animator->objects, id);
			
    		if (objDel != NULL) {
				logger->dbg("-- Anim Deleting Object: %s", objDel->name);
    			deleteObject(objDel);
    			objDel = NULL;
    		}
		}
	}

	logger->dbg("==== Animating DONE ====");
}