# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makfile                                            :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: ael-rhai <marvin@42.fr>                    +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2023/01/31 09:49:02 by ael-rhai          #+#    #+#              #
#    Updated: 2023/01/31 09:49:03 by ael-rhai         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

COMPILE = c++
FLAGS = -Wall -Wextra -Werror
NAME = Webserv
SRC = main.cpp App/Socket.cpp 
OBJ = $(SRC:.cpp=.o)

all: $(NAME)

%.o: %.cpp
	@$(COMPILE) $(CFLAGS) -c $< -o $@

$(NAME): $(OBJ)
	@$(COMPILE) $(CFLAGS) $^ -o $@
	@echo "\033[32m Compilation success\033[0m"

clean:
	@rm -rf *.o
	@echo "\033[31m object files deleted\033[0m"

fclean: clean
	@rm -rf $(NAME)
	@echo "\033[31m program terminated\033[0m"

re: fclean all