# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: vincentbaron <vincentbaron@student.42.f    +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2022/03/01 13:03:47 by vincentbaro       #+#    #+#              #
#    Updated: 2022/03/10 18:57:21 by daprovin         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #


DIR_S = srcs/
DIR_O = objs

SOURCES =   main.cpp \
			utils.cpp \


SRCS = $(addprefix $(DIR_S)/,$(SOURCES))
OBJ = $(addprefix $(DIR_O)/,$(SOURCES:.cpp=.o))
OBJS := $(filter-out objs/test_unit.o, $(OBJ))
OBJS_TEST := $(filter-out objs/main.o, $(OBJ))
HEADERS = headers

NAME = webserv
NAME_TEST = unit-test

CC = clang++

CFLAGS = -Wall -Wextra -Werror -std=c++98 -g3 #-fsanitize=address

all: $(NAME)

$(DIR_O)/%.o: $(DIR_S)/%.cpp
	mkdir -p $(DIR_O)	
	$(CC) $(CFLAGS) -o $@ -c $<

$(NAME): $(OBJS)
		$(CC) $(CFLAGS) $^ -o $@

unit-test: $(NAME_TEST)

$(DIR_O)/%.o: $(DIR_S)/%.cpp
	mkdir -p $(DIR_O)	
	$(CC) $(CFLAGS) -o $@ -c $<

$(NAME_TEST): $(OBJS_TEST)
		$(CC) $(CFLAGS) $^ -o $@

clean:
		rm -f $(OBJS)		
	rm -f $(OBJS)/*.o
	rm -rf $(DIR_O)	

fclean: clean
	rm -rf $(NAME)
	rm -rf $(NAME_TEST)

re: fclean all

.PHONY: re fclean all clean
