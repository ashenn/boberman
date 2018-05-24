#!/bin/bash
gcc -g main.c base/basic.c base/libList.c base/libParse.c base/logger.c base/math.c core/object.c core/view.c core/animation.c -I/usr/include/sdl -lSDL -lSDL_image -lSDL_ttf -o app