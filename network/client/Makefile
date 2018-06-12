CC		=   	gcc
NAME	=   	bomberman_client
SRC		=   	main.c 		\
				readline.c 	\
				commands.c 	\
				init.c 		\
				reader.c

						
OBJ		=	$(SRC:%.c=%.o)
RM		= 	rm -f
LIB		= 	-lmy_extended

$(NAME):	$(OBJ)
			$(CC) $(OBJ) -L../libs $(LIB) -o $(NAME)
all:		$(NAME)

clean:
			$(RM) $(OBJ)

fclean:		clean
			$(RM) $(NAME)

re:			fclean all
