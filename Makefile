NAME	=	codexion

FLAGS	=	-Wall -Wextra -Werror -pthread -g

SRC	=	main.c error.c utils.c getters_setters.c parse_arguments.c initialise_data.c \
		simulation.c log.c linked_list.c queue.c monitor.c wait.c clean.c

OBJ	=	$(SRC:.c=.o)

all: $(NAME)

%.o: %.c
	cc $(FLAGS) -c $< -o $@

$(NAME): $(OBJ)
	cc $(FLAGS) $(OBJ) -o $(NAME)

clean:
	rm -f $(OBJ)

fclean: clean
	rm -f $(NAME)

re: fclean all
