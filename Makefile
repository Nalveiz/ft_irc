NAME = ft_irc
SRCS = main.cpp ./src/Server.cpp
COMPILER = c++
FLAGS = -std=c++98 -Wall -Wextra -Werror -pedantic
OBJS = $(SRCS:.cpp=.o)

all: $(NAME)

$(NAME): $(OBJS)
	$(COMPILER) $(FLAGS) $(SRCS) -o $(NAME)

clean:
	rm -f $(OBJS)

fclean: clean
	rm -rf $(NAME)

re: fclean all

.PHONY: all clean fclean re
