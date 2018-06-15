CC	=	gcc
NAME	=	bomberman
SRC	=	main.c                  \
                base/basic.c            \
                base/libList.c          \
                base/libParse.c         \
                base/logger.c           \
                base/math.c             \
                core/asset.c            \
                core/event.c            \
                core/game.c             \
                core/collision.c        \
                core/object.c           \
                core/view.c             \
                core/animation.c        \
                core/player.c           \
                core/bomb.c             \
                core/map.c              \
                core/bonus.c            \
                

OBJ	=	$(SRC:%.c=%.o)
RM	=	rm -f
LIB	=	-lSDL                   \
                -lSDL_image             \
                -lSDL_ttf               \
                -lpthread               \
                -fsanitize=thread       \
                -D_REENTRANT

$(NAME):	$(OBJ)
		$(CC) $(OBJ) -L../libs $(LIB) -o $(NAME)
all:		$(NAME)

clean:
		$(RM) $(OBJ)

fclean:		clean
		$(RM) $(NAME)

re:		fclean all