CC	=	gcc
NAME	=	bomberman_server
SRC	=	main.c			        \
                network/socket.c                \
                network/handler.c               \
                network/communication.c         \
                protocol/reader.c               \
                commands/init.c                 \
                commands/login.c                \
                commands/messages.c             \
                commands/sync.c                 \
                commands/channels.c

OBJ	=	$(SRC:%.c=%.o)
RM	=	rm -f
CFLAGS  =       -W -Wall -Werror
LIB	=	-lmy_extended

$(NAME):	$(OBJ)
		$(CC) $(OBJ) -L../libs $(LIB) -o $(NAME)
all:		$(NAME)

clean:
		$(RM) $(OBJ)

fclean:		clean
		$(RM) $(NAME)

re:		fclean all
