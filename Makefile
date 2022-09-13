NAME	= webserv

SRC_DIR	= src
OBJ_DIR	= obj
INC_DIR	= include

CDEBUG	= -g -DPARSER_DEBUG

SRCS	= $(notdir $(wildcard $(SRC_DIR)/*.cpp))
SRCS	+= main.cpp
OBJS	= $(SRCS:%.cpp=$(OBJ_DIR)/%.o)
DEPs	= $(OBJS:%.o=%.d)

CC		= c++
CFLAGS	= -Wall -Wextra -Werror -std=c++98 -pedantic-errors $(CDEBUG) $(IFLAGS)
LDFLAGS	=
IFLAGS	= -I./$(INC_DIR)

VPATH	= $(SRC_DIR)

RM		= rm -f

.PHONY: all clean fclean re

$(NAME): $(OBJS)
		@$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^
		@echo "Build succesfully!"

-include $(DEPS)

$(OBJS): $(OBJ_DIR)/%.o: %.cpp
		@echo "Compiling $<"
		@mkdir -p $(@D)
		@$(CC) $(CFLAGS) -MMD -o $@ -c $<

all: $(NAME)

clean:
		@$(RM) -r $(OBJ_DIR)

fclean: clean
		@$(RM) $(NAME)

re: clean all
