# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: hchakoub <marvin@42.fr>                    +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2023/03/08 11:36:46 by hchakoub          #+#    #+#              #
#    Updated: 2023/04/02 17:36:45 by hchakoub         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

CPP=c++
FLAGS=-Wall -Wextra -Werror -std=c++98
NAME=webserv
INTRY=main.cpp
TEST_ENTRY=Local/main.cpp
SRC=App/Config.cpp App/Server.cpp HTTP/Request.cpp HTTP/Response.cpp \
		Utils/helpers.cpp dev/dev.cpp Utils/Tockenizer.cpp  App/Client.cpp \
		App/Settings/Settings.cpp \
		HTTP/Exceptions/BadRequestException.cpp \
		Bootstrap/app.cpp
BUILD_SRC=$(INTRY) $(SRC)
TEST_SRC=$(TEST_ENTRY) $(SRC)
OBJ_DIR=objects
OBJ := $(patsubst %.cpp,$(OBJ_DIR)/%.o,$(BUILD_SRC))
TEST_OBJ := $(patsubst %.cpp,$(OBJ_DIR)/%.o,$(TEST_SRC))

all: $(NAME)

test: $(TEST_OBJ)  
	$(CPP) $^ -o $(NAME) 
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
