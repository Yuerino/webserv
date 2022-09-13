NAME	= webserv

INC_DIR	= ./include

CC		= c++
# https://stackoverflow.com/questions/12606713/enforcing-the-c98-standard-in-gcc
CFLAGS	= -Wall -Wextra -Werror -std=c++98 -pedantic-errors -I$(INC_DIR) -DPARSER_DEBUG

SRCS	= Tokenizer.cpp Parser.cpp ServerConfig.cpp LocationConfig.cpp

SRC_DIR	= ./src
SRCS	:= $(addprefix $(SRC_DIR)/,$(SRCS))
SRCS	+= main.cpp

RM		= rm -f

.PHONY: all clean fclean re

$(NAME): $(SRCS)
		$(CC) $(CFLAGS) $(SRCS) -o $(NAME)

all: $(NAME)

clean:
		@$(RM) $(NAME)

fclean: clean

re: clean all
