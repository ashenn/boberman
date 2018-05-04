#include "animation.h"
#include <stdio.h>
#include <math.h>

Animator* getAnimator() {
	static Animator* animator = NULL;

	if (animator != NULL) {
		return animator;
	}

	logger->inf("First Init Animator !!");
	animator = malloc(sizeof(Animator));
	animator->objects = initListMgr();
	
	return animator;
}

AnimDistance animDistanceByFrame(int dist, float time) {
	int rest;
	int frames;
	int perFrame;
	AnimDistance animDist;

	logger->war("Dist = %d | Time %f Frame %f", dist,time, (FPS * time));
	if ((FPS * time) < 1){
		frames = 1;
	}
	else {
		frames = (FPS * time);
	}

	logger->war("AnimDist dist = %d | Frame %d", dist, frames);

	perFrame = (dist / frames);
	rest = (dist % frames);
	logger->war("1");
	
	logger->war("AnimDist Frame = %d | Rest = %d", perFrame, rest);
	logger->war("2");

	animDist.perFrame = perFrame + (rest / frames);
	logger->war("3");
	animDist.rest = rest % frames;

	logger->err("AnimDist Frame = %d | Rest = %d", animDist.perFrame, animDist.rest);

	return animDist;
}

void animSetPostion(Object* obj, int x, int y) {
	logger->err("Animation: Obj = %s |  X = %d  | Y = %d", obj->name, x, y);
	obj->pos.x = x;
	obj->pos.y = y;
}

void* animMoveTo(AnimParam* param) {
	logger->war(
		"Animation: Moving ID: #%d Obj: %s\nX = %d\nY = %d",
		param->id,
		param->obj->name,
		param->obj->pos.x,
		param->obj->pos.y
	);
	
	int xMove = param->xDist.perFrame;
	int yMove = param->yDist.perFrame;

	if (param->xDist.rest != 0) {
		logger->war("Moving X Rest = %d", param->xDist.rest);
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
		logger->war("Moving Y Rest = %d", param->yDist.rest);
		if (param->yDist.rest > 0) {
			yMove++;
			param->yDist.rest--;
		}
		else{
			yMove--;
			param->yDist.rest++;
		}

	}
	
	logger->war("Moving Coords: X = %d | Y = %d", xMove, yMove);

	animSetPostion(
		param->obj, 
		param->obj->pos.x + xMove,
		param->obj->pos.y + yMove
	);

	if (param->obj->childObj != NULL) {
		animSetPostion(
			param->obj->childObj, 
			param->obj->childObj->pos.x + xMove,
			param->obj->childObj->pos.y + yMove
		);
	}
}

void animRemoveObject(Object* obj) {
	Animator* animator = getAnimator();
	Node* objNode = getNodeByName(animator->objects, obj->name);
	if (objNode == NULL) {
		return;
	}

	deleteList(objNode->value);
	deleteNode(animator->objects, objNode->id);
}

void animAddObject(Object* obj, AnimParam* param) {
	Animator* animator = getAnimator();
	logger->inf("Adding Animation For: %s", obj->name);

	Node* objNode = getNodeByName(animator->objects, obj->name);
	if (objNode == NULL) {
		logger->inf("Animation: Adding New Object: %s", obj->name);
		objNode = addNodeV(animator->objects, obj->name, initListMgr(), 0);
	}

	Node* paramNode = addNodeV(objNode->value, obj->name, param, 1);
	logger->inf("Animation: Added %s #%d", param->obj->name, paramNode->id);
	param->id = paramNode->id;
}

void moveTo(Object* obj, int x, int y, float time, float delay) {
	logger->inf("Animation: Moving: %s to %d | %d (%fs)", obj->name, x, y, time);
	SDL_Rect target;
	target.x = x;
	target.y = y;

	vector vec = getVector(obj->pos, target);
	logger->inf("Vector: %d | %d", vec.x, vec.y);

	AnimParam* param = malloc(sizeof(AnimParam));

	param->obj = obj;
	param->time = time;
	param->delay = delay * FPS;
	param->frames = FPS * time;
	logger->inf("frames: %d", param->frames);
	logger->inf("time: %d", time);

	int xDist = getDistance(vec.x, target.x);
	//logger->inf("xDist: %d", xDist);
	
	int yDist = getDistance(vec.y, target.y);
	//logger->inf("yDist: %d", yDist);

	param->xDist = animDistanceByFrame(vec.x, time);
	logger->inf("xTotDist: %d", param->xDist);
	
	param->yDist = animDistanceByFrame(vec.y, time);
	logger->inf("yTotDist: %d", param->yDist);

	param->fnc = (void*)animMoveTo;
	logger->inf("Param Ready");

	animAddObject(obj, param);
}


void animate() {
	Node* objNode = NULL; 
	Node* paramNode = NULL; 
	Animator* animator = getAnimator(); 
	ListManager* objList = NULL;

	logger->inf("Animation: --- Animating ---");
	while((objNode = listIterate(animator->objects, objNode)) != NULL) {
		logger->err("Animation: --Loop");
	    objList = (ListManager*) objNode->value;
		logger->inf("Animation: Object: %s", objNode->name);

    	paramNode = NULL;		
	    while((paramNode = listIterate(objList, paramNode)) != NULL) {
	    	AnimParam* param = (AnimParam*) paramNode->value;
			// logger->inf("Animation: #%d %s", paramNode->id, param->obj->name);

	    	if (param->delay > 0) {
	    		param->delay--;
	    		break;
	    	}

	    	param->fnc(param);

	    	param->frames--;
			// logger->inf("Animation: Frames Left: %d", param->frames);
	    	if (param->frames <= 0){
				// logger->inf("Animation: #%d END", paramNode->id);
	    		deleteNode(objNode->value, param->id);

	    		if (!objList->nodeCount) {
					// logger->inf("Animation: No More Anim For Object: %s", objNode->name);
	    			deleteNode(animator->objects, objNode->id);
	    		}
	    	}

	    	break;
	    }
		// logger->inf("Animation: End Loop Animation");
	}
	logger->inf("Animation: --- Animating END ---");
}