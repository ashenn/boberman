#!/bin/bash
gcc -g \
main.c \
base/basic.c \
base/libList.c \
base/libParse.c \
base/logger.c \
base/math.c \
core/asset.c \
core/event.c \
core/game.c \
core/object.c \
core/view.c \
core/animation.c \
core/player.c \
-lSDL -lSDL_image -lSDL_ttf \
-fsanitize=address \
-o app