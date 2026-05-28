/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   webserver.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tjacquel <tjacquel@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/25 17:24:16 by bchallat          #+#    #+#             */
/*   Updated: 2026/04/25 18:12:51 by tjacquel         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <csignal>
#include <cstdlib>
#include <iostream>

# include "../include/webserver.hpp"

volatile sig_atomic_t g_should_exit = 0;
static void inf_loop(std::vector<int> ports);

static void signal_handler(int signal)
{
  if (signal == SIGINT)
  {
    std::cout << COLOR_YELLOW << " \nWarning : Reçu SIGINT (Ctrl+C), arrêt du serveur... " << COLOR_RESET << std::endl;
    g_should_exit = 1;
  }
}

static void initVerctorOfPort(std::vector<int>& ports)
{
  ports.push_back(8080);
  //ports.push_back(8081);
  //ports.push_back(8082);
}

int main (int av, char *ag[])
//int main()
{
	if (av != 2) {
		std::cout << "c'est pas bon kevin" << std::endl;
		return (1);
	}
	std::signal(SIGINT, signal_handler);// Enregistrer le gestionnaire de signal
	std::string filename  = ag[1];

	std::ifstream inFile(filename.c_str());
	if (!inFile.is_open()) {
		std::cerr << "Error: cannot open input file " << filename << std::endl;
		return (1);
	}

	std::string line;
	std::vector<std::string>liiines;
	while (std::getline(inFile, line)) {
		size_t  hashPos = line.find("#");
		if (hashPos != std::string::npos) {
		line = line.substr(0, hashPos);
		}
		liiines.push_back(line);
	}
	inFile.close();

	std::vector<std::string> tokens;
	for (size_t i = 0; i < liiines.size(); i++) {
		size_t j = 0;
		while (j < liiines[i].size()) {
			while (j < liiines[i].size() && isspace(liiines[i][j])) { // skip first whitespace
				j++;
			}

			if (j >= liiines[i].size()) {
				break ;
			}

			if (liiines[i][j] == '{' || liiines[i][j] == '}' || liiines[i][j] == ';') {
				tokens.push_back(liiines[i].substr(j, 1));
				j++;
				continue ;
			}

			size_t	nbegin = j;
			while (j < liiines[i].size() && !isspace(liiines[i][j]) && liiines[i][j] != '{' && liiines[i][j] != '}' && liiines[i][j] != ';') {
				j++;
			}
			tokens.push_back(liiines[i].substr(nbegin, j - nbegin));


		}

		std::cout << liiines[i] << std::endl;
	}

	if (tokens.empty()) {
		std::cerr << "Config file error: file is empty or contains no valid tokens" << std::endl;
		return (1);
	}

	for (size_t i = 0; i < tokens.size(); i++) {
		std::cout << "token[" << i << "]=" << "`" << tokens[i] << "`" << std::endl;
	}

	int	bracket_count = 0;
	for (size_t i = 0; i < tokens.size(); i++) {

		if (tokens[i] == "{") {
			bracket_count++;
		}
		if (tokens[i] == "}") {
			bracket_count--;
		}
		if (bracket_count < 0) {
			std::cerr << "Error: too many closing brackets in config file" << std::endl;
			return (1);
		}
		}
		if (bracket_count != 0) {
			std::cerr << "Config file error: closing bracket count doest not match open bracket count" << std::endl;
			return (1);
	}

	bracket_count = 0;
	bool	in_brackets = false;
	for (size_t i = 0; i < tokens.size(); i++) {
		if (tokens[i] == "{") {
			bracket_count++;
			in_brackets = true;
		}
		if (tokens[i] == "}") {
			bracket_count--;
			if (bracket_count == 0) {
				in_brackets = false;
			}
		}
		if (in_brackets == false && tokens[i] != "server" && tokens[i] != "}") {
			std::cerr << "Config file error: only `server` token expected outside brackets block but found tokens[" << i << "] == " << tokens[i] << std::endl;
			std::cout << "in_brackets=" << in_brackets << " and bracket_count=" << bracket_count << std::endl;
			return (1);
		}

		if (i > 0 && tokens[i] == ";" && tokens[i - 1] == ";") {
			std::cerr << "Config file error: consecutive separating operator `;` found" << std::endl;
			return (1);
		}

		if (i > 0 && tokens[i] == "{" && tokens[i - 1] == "{") {
			std::cerr << "Config file error: consecutive open bracket found" << std::endl;
			return (1);
		}

		if (i > 0 && tokens[i] == "{" && tokens[i - 1] == ";") {
			std::cerr << "Config file error: unexpected `;` found before opening brackets `{`" << std::endl;
			return (1);
		}
	}

	std::cout << "tokens.back()=" << tokens.back() << std::endl;
	if (tokens.back() != "}") {
		std::cerr << "Config file error: last token is not a closing bracket" << std::endl;
		return (1);
	}
	// std::cout << "directiveblocks_count=" << directiveblocks_count << std::endl;

	// std::vector<t_block>	parser;
	// t_block buffer;
	// in_brackets = false;
	// bracket_count = 0;
	// for (size_t i = 0; i < tokens.size(); i++) {
	// 	if (i + 1 < tokens.size() && tokens[i] == "server") {
	// 		buffer.context = tokens[i];
	// 		parser.push_back(buffer);
	// 	}
	// 	if (i + 2 < tokens.size() && tokens[i] == "location") {
	// 		buffer.context = tokens[i];
	// 		parser.push_back(buffer);
	// 	}
	// 	// else if (i > 0 && tokens[i] == ";" ) {
	// 	// 	size_t sc_pos = i;
	// 	// 	size_t	j = i;
	// 	// 	while (j > 0 && tokens[j] != ";" && tokens[j] != "}" && tokens[j] != "{") {
	// 	// 		j--;
	// 	// 	}
	// 	// 	j++;
	// 	// 	size_t nbegin = j;
	// 	// 	while (j < sc_pos - nbegin) {
	// 	// 		buffer.directives.push_back(tokens[j]);
	// 	// 		// parser[i].directives.push_back(tokens[j]);
	// 	// 		j++;
	// 	// 	}
	// 	// 	parser[i].directives.push_back(buffer.directives[i]);
	// 	// 	// buffer.directives
	// 	// }
	// }

	std::vector<t_block>	block;
	size_t i = 0;
	while (i < tokens.size()) {
		if (tokens[i] == "server") {
			block.push_back(parseTokens(tokens, i));
		}
		else {
			std::cerr << "Config file error: Unexpected token found outside server block: tokens[" << i << "]=" << tokens[i] << std::endl;
			return (1);
		}
	}

	// std::cout << "block.size()=" << block.size() << std::endl;
	// for (size_t i = 0; i < block.size(); i++) {
	// 	std::cout << "block[" << i << "].name=" << block[i].name << std::endl;
	// 	for (size_t j = 0; j < block[i].parameters.size(); j++) {
	// 		std::cout << "block[" << i << "].parameters[" << j << "]=" << block[i].parameters[j] << std::endl;
	// 	}
	// 	for (size_t k = 0; k < block[i].directives.size(); k++) {
	// 		for (size_t ka = 0; ka < block[i].directives[k].size(); ka++) {
	// 			std::cout << "    block[" << i << "].directives[" << k << "][" << ka << "]=" << block[i].directives[k][ka] << std::endl;
	// 		}
	// 	}
	// 	for (size_t l = 0; l < block[i].embed_block.size(); l++) {
	// 		std::cout << "    block[" << i << "].embed_block[" << l << "].name=" << block[i].embed_block[l].name << std::endl;
	// 		for (size_t la = 0; la < block[i].embed_block[l].parameters.size(); la++) {
	// 			std::cout << "    block[" << i << "].embed_block[" << l << "].parameters[" << la << "]=" << block[i].embed_block[l].parameters[la] << std::endl;
	// 		}
	// 		for (size_t lb = 0; lb < block[i].embed_block[l].directives.size(); lb++) {
	// 			for (size_t lba = 0; lba < block[i].embed_block[l].directives[lb].size(); lba++) {
	// 				std::cout << "        block[" << i << "].embed_block[" << l << "].directives[" << lb << "][" << lba << "]=" << block[i].embed_block[l].directives[lb][lba] << std::endl;
	// 			}
	// 		}

	// 	}
	// }

	for (size_t i = 0; i < block.size(); i++) {
		std::stringstream	ss;
		ss << "block[" << i << "]";
		std::string			base_prefix = ss.str();

		printParserBlocks(block[i], base_prefix, 0);
		std::cout << "\n";
	}


std::vector<int> ports;
initVerctorOfPort(ports);

try {
      inf_loop(ports);

    } catch (const std::exception& e) {
        
      std::cerr << COLOR_RED << "Error: " << e.what() << COLOR_RESET << std::endl;
      return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

static void handler_conection(EventLoop &loop)
{
  std::vector<int> activeFds = loop.getActiveFds();
  
  for (size_t i = 0; i < activeFds.size(); i++) 
  {
    int fd = activeFds[i];
    if (loop.isServerFd(fd)) 
    {
      loop.acceptNewConnection(fd);
    } 
    else 
    {
      char buffer[4096];
      ssize_t bytes_read = loop.readFromClient(fd, buffer, sizeof(buffer));
      if (bytes_read <= 0) 
      {
        loop.removeClient(fd);
        std::cout << COLOR_CYAN << "(Log Client) : client déconnecté (fd=" << fd << ")." << COLOR_RESET << std::endl;
        continue;
      }
      // 👇 Traitement Request HTTP
      t_httpRequest request = HttpHandler::setHttpRequest(buffer);
      t_httpResponse response = HttpHandler::setHttpResponse(request);
      std::string raw_response = response.toString();
      loop.sendResponse(fd, raw_response);
    }
  }
}

static void inf_loop(std::vector<int> ports)
{
  EventLoop loop(ports);
  loop.setupServerSockets();

  while (!g_should_exit) 
  {
    int ret = loop.waitForActivity();
    if (ret == -1)
    {
      if(!g_should_exit)
        break ;
      std::cerr << COLOR_RED << "Error: poll() failed (interrupted system call?)" << COLOR_RESET << std::endl;
      break;
    }
    else
    {
      handler_conection(loop);
    }
  }
  loop.cleanup();
}

/*

while (true)
{

  Attendre une activité sur les sockets (timeout infini)

  Parcourir tous les descripteurs surveillés

  Vérifier si c'est un socket serveur (nouvelle connexion)
    Sinon, c'est un client existant (données à lire)
}
*/
