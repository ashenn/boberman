#ifndef __MAIN_H__
#define __MAIN_H__

#include "common.h"

void handleEvents();
void tickWait(int next);
void* closeApp();

void lockLogger(int flag);
void unlockLogger(int flag);

void lock(int flag);
void unlock(int flag);
void enableLogger(int flag);

#endif