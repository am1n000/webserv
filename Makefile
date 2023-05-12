# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: hchakoub <marvin@42.fr>                    +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2023/03/08 11:36:46 by hchakoub          #+#    #+#              #
#    Updated: 2023/05/12 20:19:46 by hchakoub         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

CPP=c++
FLAGS=-Wall -Wextra -Werror -std=c++98
NAME=webserv
INTRY=main.cpp
TEST_ENTRY=main.cpp
SRC=App/Config.cpp App/Server.cpp HTTP/Request.cpp HTTP/Response.cpp \
		Utils/helpers.cpp dev/dev.cpp Utils/Tockenizer.cpp  App/Client.cpp \
		App/Settings/Settings.cpp App/multiplexers/Kqueue.cpp\
		Bootstrap/app.cpp App/multiplexers/Select.cpp \
		App/multiplexers/Poll.cpp \
		Bootstrap/app.cpp \
		App/Cgi.cpp \
		HTTP/Exceptions/HttpNotImplementedException.cpp \
		Utils/autoIndex.cpp
BUILD_SRC=$(INTRY) $(SRC)
TEST_SRC=$(TEST_ENTRY) $(SRC)
OBJ_DIR=objects
OBJ := $(patsubst %.cpp,$(OBJ_DIR)/%.o,$(BUILD_SRC))
TEST_OBJ := $(patsubst %.cpp,$(OBJ_DIR)/%.o,$(TEST_SRC))

all: makedir $(NAME)

test: makedir build_test

build_test: $(TEST_OBJ)  
	$(CPP) $^ -o $(NAME) 
	@clear
	@./$(NAME)

$(OBJ_DIR)/%.o:%.cpp
	$(CPP) $(FLAGS) -c $< -o $@

$(NAME): $(OBJ)
	$(CPP) $^ -o $@

debug: $(SRC)
	$(CPP) -g $^ main.cpp  -o $@
	clear
	lldb debug

sanitize: $(TEST_OBJ)  
	$(CPP) $^ -fsanitize=address -o $(NAME) 
	@clear
	@./$(NAME)


makedir:
	mkdir -p objects
	mkdir -p objects/App/
	mkdir -p objects/App/multiplexers
	mkdir -p objects/Utils/
	mkdir -p objects/dev/
	mkdir -p objects/App/Settings/
	mkdir -p objects/Bootstrap/
	mkdir -p objects/HTTP/
	mkdir -p objects/Local/
	mkdir -p objects/HTTP/Exceptions/

rmdir:
	rm -rf objects

clean:
	rm -rf $(OBJ)
	rm -rf debug

fclean: clean rmdir
	rm -rf $(NAME)

re: fclean all
