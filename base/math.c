#include "math.h"

vector getVector(SDL_Rect rect1, SDL_Rect rect2) {
	vector vec;
	vec.x = rect2.x - rect1.x;
	vec.y = rect2.y - rect1.y;

	return vec;
}

int getDistance(int x, int y) {
	return y - x;
}

int vectorDistance(vector* vec) {
	return getDistance(vec->x, vec->y);
}