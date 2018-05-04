#ifndef _MATH_H__
#define _MATH_H__

#include <math.h>
#include <stdlib.h>
#include <SDL/SDL.h>

typedef struct vector vector;
struct vector {
	int x;
	int y;
};

int getDistance(int x, int y);
int vectorDistance(vector* vec);
vector getVector(SDL_Rect rect1, SDL_Rect rect2);

#endif