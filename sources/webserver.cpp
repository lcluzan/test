/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   webserver.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lcluzan <lcluzan@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/25 17:24:16 by bchallat          #+#    #+#             */
/*   Updated: 2026/06/10 14:55:48 by bchallat         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <csignal>
#include <cstdlib>
#include <iostream>

# include "../include/webserver.hpp"


/* ========================================================================== */
/*                    -- definition and prototype --                          */
/* ========================================================================== */
#if RUN_WITH_SERVER

volatile sig_atomic_t g_should_exit = 0;

static void           signal_handler(int signal);
static void           print_http_request(const t_httpRequest& request); 
static void           print_http_response(const t_httpResponse& response); 
std::string           readFullHttpRequest(int fd, EventLoop& loop) ;
static void           inf_loop(std::vector<int> ports, const std::vector<ServerConfig>& config);
static int            getPortServer(int fd);
static void           initVerctorOfPort(std::vector<int>& ports, const std::vector<ServerConfig>& virtual_servers);
static ServerConfig  getConfigForHandler(const std::vector<ServerConfig>& vec_config, int port);

#endif
/* ========================================================================== */
/*                           -- main focntion --                              */
/* ========================================================================== */

int main (int av, char *ag[]) 
{
	if (av != 2) {

		std::cout << "Usage: ./webserv [configuration file]" << std::endl;
		return (EXIT_FAILURE);
	}

#if RUN_WITH_SERVER
		std::signal(SIGINT, signal_handler);// Enregistrer le gestionnaire de signal
#endif

try {
	std::vector<ServerConfig>	virtual_servers;
	confParsingHandler(ag[1], virtual_servers);

#if RUN_WITH_SERVER
	std::vector<int> ports;
	initVerctorOfPort(ports, virtual_servers);
	inf_loop(ports, virtual_servers);
#endif

    } catch (const std::exception& e) {

      std::cerr << COLOR_RED << "Error: " << e.what() << COLOR_RESET << std::endl;
      return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}


/* ========================================================================== */
/*                           -- Loop of event --                              */
/* ========================================================================== */
#if RUN_WITH_SERVER


static ServerConfig  getConfigForHandler(const std::vector<ServerConfig>& vec_config, int port)
{
  ServerConfig  config;

  for (std::vector<ServerConfig>::const_iterator it = vec_config.begin(); it != vec_config.end(); it++) {
    
    if (it->getPort() == port) {

      config = *it;
      return ( config );
    }
  }

  return ( config );
}

static void handler_conection(EventLoop &loop, const std::vector<ServerConfig>& vec_config)
{
  std::vector<int> activeFds = loop.getActiveFds();

  for (size_t i = 0; i < activeFds.size(); i++)
  {
    int fd = activeFds[i];
    if (loop.isServerFd(fd)) {

      loop.acceptNewConnection(fd);
    } else {

      std::string string_request;
      string_request = readFullHttpRequest(fd, loop);
      
      ServerConfig config = getConfigForHandler(vec_config, getPortServer(fd));

      std::cout << "Server port for fd=" << fd << ": " << getPortServer(fd) << std::endl;

      t_httpRequest request = HttpHandler::setHttpRequest(string_request);
      print_http_request(request);
      t_httpResponse response = HttpHandler::setHttpResponse(request, config);

      std::string raw_response = response.toString();
      loop.sendResponse(fd, raw_response);

      if (response.status == 400)
      {
        loop.removeClient(fd);
      }

      print_http_response(response);

    }
  }
}

static void inf_loop(std::vector<int> ports, const std::vector<ServerConfig>& vec_config)
{
  int       ret = -1;
  EventLoop loop(ports);

  loop.setupServerSockets();
  while (!g_should_exit) {

    ret = loop.waitForActivity();
    if (ret == -1)
      std::cerr << COLOR_RED << "Error: poll() failed (interrupted system call?)" << COLOR_RESET << std::endl;
    else
      handler_conection(loop,vec_config);
  }
}

/* ========================================================================== */
/*                                                                            */
/* ========================================================================== */

static void signal_handler(int signal)
{
  if (signal == SIGINT)
  {
    std::cout << COLOR_YELLOW << " \nWarning : Reçu SIGINT (Ctrl+C), arrêt du serveur... " << COLOR_RESET << std::endl;
    g_should_exit = 1;
  }
}

	// std::cout << "virtual_servers.size()=" << virtual_servers.size() << std::endl;
		// std::cout << "virtual_servers[i].getPort()=" << virtual_servers[i].getPort() << std::endl;
static void initVerctorOfPort(std::vector<int>& ports, const std::vector<ServerConfig>& virtual_servers)
{
	for (size_t i = 0; i < virtual_servers.size(); ++i)
    ports.push_back(virtual_servers[i].getPort());
}

static int  getPortServer(int fd)
{
  struct sockaddr_in server_addr;
  socklen_t server_addr_len = sizeof(server_addr);
  
  if (getsockname(fd, (struct sockaddr *)&server_addr, &server_addr_len) == -1)
        std::cerr << COLOR_RED << "getsockname() failed: " << strerror(errno) << COLOR_RESET << std::endl;

  return ( ntohs(server_addr.sin_port) );
}

/* ========================================================================== */
/*                 -- print fonction for request/response --                  */
/* ========================================================================== */

static void print_http_request(const t_httpRequest& request) {
    // Affichage de la méthode, du chemin et de la version
    std::cout << COLOR_CYAN << "\n=== HTTP Request ===" << COLOR_RESET << std::endl;
    std::cout << "Method: " << COLOR_YELLOW << request.method << COLOR_RESET << std::endl;
    std::cout << "Path: " << COLOR_YELLOW << request.path << COLOR_RESET << std::endl;
    std::cout << "Version: " << COLOR_YELLOW << request.version << COLOR_RESET << std::endl;

    // Affichage des en-têtes
    std::cout << COLOR_CYAN << "\n--- Headers ---" << COLOR_RESET << std::endl;
    if (request.headers.empty()) {
        std::cout << COLOR_RED << "No headers." << COLOR_RESET << std::endl;
    } else {
        for (std::map<std::string, std::string>::const_iterator it = request.headers.begin();
             it != request.headers.end(); ++it) {
            std::cout << it->first << ": " << COLOR_GREEN << it->second << COLOR_RESET << std::endl;
        }
    }
    std::cout << COLOR_CYAN << "===================\n" << COLOR_RESET << std::endl;
}

static void print_http_response(const t_httpResponse& response) {
    // Affichage du statut
    std::cout << COLOR_CYAN << "\n=== HTTP Response ===" << COLOR_RESET << std::endl;
    std::cout << "Status: " << COLOR_YELLOW << response.status << COLOR_RESET;
    switch (response.status) {
        case 200: std::cout << " (OK)"; break;
        case 400: std::cout << " (Bad Request)"; break;
        case 403: std::cout << " (Forbidden)"; break;
        case 404: std::cout << " (Not Found)"; break;
        case 501: std::cout << " (Not Implemented)"; break;
        default: std::cout << " (Unknown Status)"; break;
    }
    std::cout << COLOR_RESET << std::endl;

    // Affichage des en-têtes
    std::cout << COLOR_CYAN << "\n--- Headers ---" << COLOR_RESET << std::endl;
    if (response.headers.empty()) {
        std::cout << COLOR_RED << "No headers." << COLOR_RESET << std::endl;
    } else {
        for (std::map<std::string, std::string>::const_iterator it = response.headers.begin();
             it != response.headers.end(); ++it) {
            std::cout << it->first << ": " << COLOR_GREEN << it->second << COLOR_RESET << std::endl;
        }
    }
    std::cout << COLOR_CYAN << "====================\n" << COLOR_RESET << std::endl;
}

/* ========================================================================== */
/*                   -- fonc for read full request --                         */
/* ========================================================================== */

// Helper pour extraire le Content-Length proprement
static size_t extractContentLength(const std::string& request) {
    size_t pos = request.find("Content-Length: ");
    if (pos != std::string::npos) {
        size_t start = pos + 16; // "Content-Length: " fait 16 caractères
        size_t end = request.find("\r\n", start);
        if (end != std::string::npos) {
            return static_cast<size_t>(std::atoi(request.substr(start, end - start).c_str()));
        }
    }
    return 0;
}

std::string readFullHttpRequest(int fd, EventLoop& loop) {
    std::string request;
    char buffer[BUFFER_SIZE]; // Utilisation de la macro définie dans webserver.hpp
    size_t header_end_pos = std::string::npos;
    size_t content_length = 0;

    while (true) {
        ssize_t bytes_read = loop.readFromClient(fd, buffer, sizeof(buffer));

        if (bytes_read <= 0) {
            if (bytes_read == 0)
                std::cout << COLOR_RED << "Client disconnected (fd=" << fd << ")" << COLOR_RESET << std::endl;
            else
                std::cerr << "Error reading from client (fd=" << fd << ")" << std::endl;
            return ""; // Abandonne la requête en cas d'erreur
        }

        request.append(buffer, bytes_read);

        // Cherche la fin des headers seulement si on ne l'a pas encore trouvée
        if (header_end_pos == std::string::npos) {
            header_end_pos = request.find("\r\n\r\n");

            if (header_end_pos != std::string::npos) {
                content_length = extractContentLength(request);
            }
        }

        // Si on a les headers, on vérifie si on a reçu tout le body
        if (header_end_pos != std::string::npos) {
            size_t body_bytes_read = request.size() - (header_end_pos + 4);
            if (body_bytes_read >= content_length) {
                break; // Requête entièrement reçue !
            }
        }
    }
    return request;
}
#endif

