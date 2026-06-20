# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: lcluzan <lcluzan@student.42.fr>            +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2026/02/20 09:10:14 by bchallat          #+#    #+#              #
#    Updated: 2026/06/20 04:32:25 by lcluzan          ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

#=======================================================================#
#                                                                       #
#=======================================================================#

GREEN	= \033[0;32m
YELLOW	= \033[0;33m
BLUE	= \033[0;34m
RED 	= \033[0;91m
RESET	= \033[0m

UNAME := $(shell uname)
ifeq ($(UNAME), Darwin)
	ECHO_FLAG	=	echo
	CC				= clang++
	FLAG			= -g3 -Wall -Wextra -Werror -std=c++98
else
	ECHO_FLAG	= echo -e
	CC				= c++
	FLAG			= -Wall -Wextra -Werror -std=c++98 -g -pedantic
endif

CCFLAG	= $(CC) $(FLAG) -I./include -MMD -MP

SRCDIR = sources
OBJDIR = object

RMF = rm -rf

#=======================================================================#
#                                                                       #
#=======================================================================#

NAME = webserv

SRC = webserver.cpp ${SRC_CONFIG} ${SRC_NETWORCK} ${SRC_REQUEST} ${SRC_CGI}

SRC_CONFIG		=	config/ServerConfig.cpp \
								config/ConfigParsing.cpp \
								config/ConfigParsingUtils.cpp \
								config/LocationConfig.cpp \


SRC_NETWORCK	=	network/SocketHandler.cpp\
								network/ClientInfo.cpp \
								network/ClientManager.cpp \
								network/EventLoop.cpp \

SRC_REQUEST		=	request/HttpHandler.cpp \
								request/parsingHttpHandler.cpp \
								request/responseHttpHandler.cpp \
								request/cgiHandler.cpp \
								request/methodHttpGet.cpp \
								request/methodHttpPost.cpp \
								request/methodHttpDelete.cpp \
								request/HttpStatusCode.cpp \

SRCS = ${addprefix ${SRCDIR}/, ${SRC}}
OBJS = ${addprefix ${OBJDIR}/, $(SRC:.cpp=.o)}

#=======================================================================#
#                                                                       #
#=======================================================================#

all: $(NAME)

$(NAME): $(OBJS)
	@${ECHO_FLAG}  "\n$(GREEN)[OK]       	✅ $(YELLOW)CREATED   EXE$(BLUE) -- $(RED)${NAME} --\n $(RESET)"
	@$(CCFLAG) -g3 $(OBJS) -o $(NAME)

object/%.o:sources/%.cpp | $(OBJDIR)
	@${ECHO_FLAG}  "$(GREEN)[OK]       	🛠️ $(YELLOW)Compiling ... $(BLUE)$< $(RESET)"
	@$(CCFLAG) -c $< -o $@

${OBJDIR}:
	@${ECHO_FLAG} "${GREEN}[->]		${YELLOW}📂 Creating directory $(BLUE)object/..$(RESET)\n"
	@mkdir -p ${OBJDIR}
	@mkdir -p ${OBJDIR}/network
	@mkdir -p ${OBJDIR}/cgi
	@mkdir -p ${OBJDIR}/config
	@mkdir -p ${OBJDIR}/request
	@mkdir -p ${OBJDIR}/routing

#===============================================================#
#                                                               #
#===============================================================#

clean:
	@${ECHO_FLAG} "$(RED)[DELETE]	🧹$(BLUE) Delete file obj $(RESET)\n"
	@$(RMF) $(OBJDIR) $(OBJS)

fclean:
	@${ECHO_FLAG} "$(RED)[DELETE]	🧹$(BLUE) Delete all file obj and target $(RESET)\n"
	@$(RMF) $(OBJDIR) $(OBJS) $(NAME) $(TEST_NAME)

re: fclean all

.PHONY: all clean fclean re

