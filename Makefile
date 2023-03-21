# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: hchakoub <marvin@42.fr>                    +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2023/03/08 11:36:46 by hchakoub          #+#    #+#              #
#    Updated: 2023/03/21 21:43:04 by hchakoub         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

CPP=c++
FLAGS=-Wall -Wextra -Werror -std=c++98
NAME=webserv
SRC=main.cpp App/Config.cpp App/Server.cpp Utils/helpers.cpp dev/dev.cpp Utils/Tockenizer.cpp
OBJ_DIR=objects
OBJ := $(patsubst %.cpp,$(OBJ_DIR)/%.o,$(SRC))

all: $(NAME)

test: all  
	@clear
	@./$(NAME)

$(OBJ_DIR)/%.o:%.cpp
	$(CPP) $(FLAGS) -c $< -o $@

$(NAME): $(OBJ)
	$(CPP) $^ -o $@

debug: $(SRC)
	$(CPP) -g $^  -o $@
	clear
	lldb debug


clean:
	rm -rf $(OBJ)
	rm -rf debug

fclean: clean
	rm -rf $(NAME)

re: fclean all
