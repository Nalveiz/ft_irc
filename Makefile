NAME = ircserv
SRCS = main.cpp ./src/Server.cpp ./src/Client.cpp ./src/CommandParser.cpp ./src/CommandExecuter.cpp ./src/IRCMessage.cpp ./src/Chanell.cpp ./src/IRCResponse.cpp ./src/ModeHandler.cpp ./src/ChannelCommands.cpp
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