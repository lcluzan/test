# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: tjacquel <tjacquel@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2026/02/20 09:10:14 by bchallat          #+#    #+#              #
#    Updated: 2026/04/27 14:05:53 by bchallat         ###   ########.fr        #
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
	FLAG			= -g3 -Wall -Wextra -Werror -std=c++98 -fsanitize=address
else
	ECHO_FLAG	= echo -e
	CC				= c++
	FLAG			= -Wall -Wextra -Werror -std=c++98
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

SRC_NETWORCK	=	network/SocketHandler.cpp\
								network/ClientInfo.cpp \
								network/ClientManager.cpp \
								network/EventLoop.cpp \

SRC_REQUEST		=	request/HttpHandler.cpp \
								request/lexing.cpp \
								request/descending_deriv.cpp \
								request/struct_http_request.cpp \
								request/readFile.cpp \
								request/httpResponse.cpp \
								request/servStaticFile.cpp \
								request/isStaticFile.cpp \

SRC_CGI				=	cgi/CgiHandler.cpp \

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

TEST_NAME = t_unit

TEST_DIR = stack/tests

TEST_SRCS = $(wildcard $(TEST_DIR)/unit_test/*.cpp)
TEST_OBJS = $(patsubst $(TEST_DIR)/unit_test/%.cpp, $(OBJDIR)/%.o, $(TEST_SRCS))

#=======================================================================#
#                                                                       #
#=======================================================================#


unit_test: $(TEST_NAME)

$(TEST_NAME): $(TEST_OBJS) $(OBJS)
	@${ECHO_FLAG} "\n$(GREEN)[OK]       	🧪 $(YELLOW)CREATED   EXE$(BLUE) -- $(RED)${TEST_NAME} --\n $(RESET)"
	@$(CCFLAG) $(filter-out $(OBJDIR)/webserver.o, $(OBJS)) $(TEST_OBJS) -o $(TEST_NAME)

$(OBJDIR)/%.o: $(TEST_DIR)/unit_test/%.cpp | $(OBJDIR)
	@${ECHO_FLAG} "$(GREEN)[OK]       	🛠️ $(YELLOW)Compiling ... $(BLUE)$<$(RESET)"
	@$(CCFLAG) -Iinclude -I$(TEST_DIR)/include -c $< -o $@

	
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

up: all unit_test

.PHONY: all clean fclean re up unit_test


