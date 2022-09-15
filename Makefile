NAME		=	webserv

SRC_DIR		=	src
OBJ_DIR		=	obj
INC_DIR		=	include

SRCS		=	$(notdir $(wildcard $(SRC_DIR)/*.cpp))
OBJS		=	$(SRCS:%.cpp=$(OBJ_DIR)/%.o)
DEPS		=	$(OBJS:%.o=%.d)

CXX			=	c++
CXXFLAGS	=	-Wall -Wextra -Werror $(IFLAGS)
CXX98FLAGS	=	-std=c++98 -pedantic-errors
LDFLAGS		=
IFLAGS		=	-I./$(INC_DIR)
CDEBUG		=	-g -D PARSER_DEBUG

RM			=	rm -f

.PHONY: all clean fclean re debug run_test

$(NAME): $(OBJS) $(OBJ_DIR)/main.o
		@$(CXX) $(CXXFLAGS) $(CXX98FLAGS) $(LDFLAGS) -o $@ $^
		@echo "Build $(NAME) succesfully!"

-include $(DEPS)

$(OBJ_DIR)/%.o: %.cpp
		@echo "Compiling $<"
		@mkdir -p $(@D)
		@$(CXX) $(CXXFLAGS) $(CXX98FLAGS) -MMD -o $@ -c $<

all: $(NAME)

clean:
		@$(RM) -r $(OBJ_DIR)
		@$(RM) -r $(NAME).dSYM
		@$(RM) -r $(TEST_NAME).dSYM

fclean: clean
		@$(RM) $(NAME) $(TEST_NAME)

re: clean all

debug: CXX98FLAGS += $(CDEBUG)
debug: re

#=============================================================================#
# Unit test stuff

TEST_NAME	=	unit_test

T_SRC_DIR	=	test

T_SRCS		=	$(notdir $(wildcard $(T_SRC_DIR)/*.cpp))
T_OBJS		=	$(T_SRCS:%.cpp=$(OBJ_DIR)/%.o)

GTEST_DIR	=	test/googletest/googletest
GMOCK_DIR	=	test/googletest/googlemock

GTEST_SRCS	=	gtest_main.cc gtest-all.cc
GTEST_OBJS	=	$(GTEST_SRCS:%.cc=$(OBJ_DIR)/%.o)

T_IFLAGS	=	-isystem $(GTEST_DIR)/include -isystem $(GMOCK_DIR)/include \
				-isystem $(GTEST_DIR) -isystem $(GMOCK_DIR)

VPATH		=	$(SRC_DIR) $(T_SRC_DIR) $(GTEST_DIR)/src

run_test: $(TEST_NAME)
		./$(TEST_NAME) --gtest_brief=1

$(TEST_NAME): $(OBJS) $(GTEST_OBJS) $(T_OBJS)
		@$(CXX) $(CXXFLAGS) -pthread $(T_IFLAGS) -lpthread -o $@ $^
		@echo "Build $(TEST_NAME) succesfully!"

$(GTEST_OBJS): $(OBJ_DIR)/%.o: %.cc
		@echo "Compiling $<"
		@mkdir -p $(@D)
		@$(CXX) $(CXXFLAGS) -pthread -std=c++14 $(T_IFLAGS) -o $@ -c $<

$(T_OBJS): $(OBJ_DIR)/%.o: %.cpp
		@echo "Compiling $<"
		@mkdir -p $(@D)
		@$(CXX) $(CXXFLAGS)  -pthread -std=c++14 $(T_IFLAGS) -o $@ -c $<

#=============================================================================#
