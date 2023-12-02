# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: minjinki <minjinki@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2023/11/15 11:51:53 by minjinki          #+#    #+#              #
#    Updated: 2023/12/02 13:27:17 by minjinki         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME		=	ircserv

SRC_FILES	=	main\
				Server\
				Utils\
				Client\
				Command\
				Cmd_INVITE\
				Cmd_JOIN\
				Cmd_PART\
				Cmd_PASS\
				Cmd_PRIVMSG\
				Cmd_USER\
				Cmd_MODE\
				Channel

SRCS		=	$(addsuffix .cpp, $(SRC_FILES))

OBJS		=	$(SRCS:%.cpp=%.o)

CXX			=	c++
CPPFLAGS	=	-Wall -Wextra -Werror -std=c++98
RM			=	rm -f

all			:	$(NAME)

%.o			:	%.cpp
	$(CXX) $(CPPFLAGS) -c $< -o $@

$(NAME)		:	$(OBJS)
	$(CXX) $(CPPFLAGS) $(OBJS) -o $(NAME)

mclean		:	$(NAME)
	$(MAKE)	clean

clean		:
	$(RM) $(OBJS)

fclean		:	clean
	$(RM) $(NAME)

re			:
	$(MAKE) fclean
	$(MAKE) all

.PHONY		:	all clean fclean re