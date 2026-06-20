/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   webserver.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lcluzan <lcluzan@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/25 17:24:16 by bchallat          #+#    #+#             */
/*   Updated: 2026/06/20 03:40:09 by lcluzan          ###   ########.fr       */
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

void           print_http_request(const t_httpRequest& request);
void           print_http_response(const t_httpResponse& response);
std::string           readFullHttpRequest(int fd, EventLoop& loop) ;
static void           inf_loop(std::vector<int> ports, const std::vector<ServerConfig>& config);
static int            getPortServer(int fd);
static void           initVerctorOfPort(std::vector<int>& ports, const std::vector<ServerConfig>& virtual_servers);
static ServerConfig  getConfigForHandler(const std::vector<ServerConfig>& vec_config, int port);

#endif
void testPatch(std::vector<int> ports, const std::vector<ServerConfig>& vec_config);
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
		std::signal(SIGQUIT, SIG_IGN);
		std::signal(SIGPIPE, SIG_IGN); // a determiner si on conserve ou non
#endif

try {
	std::vector<ServerConfig>	virtual_servers;
	confParsingHandler(ag[1], virtual_servers);
#if RUN_WITH_SERVER
	std::vector<int> ports;
	initVerctorOfPort(ports, virtual_servers);
	inf_loop(ports, virtual_servers);
  //testPatch(ports, virtual_servers);
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

/*

┌──────────────── Server::run() ────────────────┐
│                                               │
│   while (_running) {                          │
│     poll(_pollFds, _nfds, TIMEOUT_MS)         │
│                                               │
│     for each fd with event:                   │
│       if fd == listenFd  → acceptClient()     │
│       if POLLIN          → handleRead(fd)     │
│       if POLLOUT         → handleWrite(fd)    │
│       if POLLERR/POLLHUP → removeClient(fd)   │
│   }                                           │
└───────────────────────────────────────────────┘

*/

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

ssize_t readAvailableData(int fd, EventLoop& loop, std::string& data) {
    char buffer[BUFFER_SIZE];
    ssize_t bytes_read = loop.readFromClient(fd, buffer, sizeof(buffer));

    if (bytes_read > 0) {
        data.append(buffer, bytes_read);
	}

    return bytes_read;
}

static void handler_conection(EventLoop &loop, const std::vector<ServerConfig>& vec_config) {

    loop.checkCgiTimeout();

    std::vector<int>	activeFds = loop.getActiveFds();

    for (size_t i = 0; i < activeFds.size(); i++) {
        int fd = activeFds[i];
        size_t pollIndex = 0;
        for (; pollIndex < loop.getPollFdsSize(); pollIndex++) {
            if (loop.getPollFd(pollIndex).fd == fd) break;
        }
        short revents = loop.getPollFd(pollIndex).revents;

		if (loop.isCgiFd(fd)) {
			loop.handleCgiEvent(fd, revents);
			continue;
		}

        if (revents & (POLLERR | POLLHUP)) {
            loop.removeClient(fd);
            continue;
        }

        if (loop.isServerFd(fd) && (revents & POLLIN)) {
            loop.acceptNewConnection(fd);
            continue;
        }

        if (revents & POLLIN) {
			std::string			data;
            ssize_t bytes_read = readAvailableData(fd, loop, data);
			if (bytes_read == 0) {
				loop.removeClient(fd); // Graceful disconnect
				continue;
			}
			else if (bytes_read == -1) {
				// If it's EAGAIN, just continue and let poll() trigger again later
				continue;
			}
			else {
				loop.appendToClientBuffer(fd, data);
			}

            if (loop.isRequestComplete(fd)) {
				std::string full_request = loop.getFullRequest(fd);

				// FIX: Ignore leading CRLFs / Empty requests
				size_t start = full_request.find_first_not_of("\r\n");
				if (start == std::string::npos) {
					continue; // It was just empty lines, ignore and go back to poll()
				}
				full_request = full_request.substr(start);

                ServerConfig config = getConfigForHandler(vec_config, getPortServer(fd));
                t_httpRequest request = HttpHandler::setHttpRequest(full_request);
				request.client_ip = loop.getClientIP(fd);
                print_http_request(request);
                t_httpResponse response = HttpHandler::setHttpResponse(request, config);
				if (response.is_cgi) { // CGI response
					loop.registerCgi(fd, response, request, config);
					if (response.headers["Connection"] == "close") {
						loop.markClientForDisconnect(fd);
					}
				}
				else { // Static response
					print_http_response(response);
					loop.queueResponse(fd, response.toString());
				}

				if (response.headers["Connection"] == "close") {
					loop.markClientForDisconnect(fd);
				}
            }
        }

        if (revents & POLLOUT) {
            loop.handlePendingWrites(fd);
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

void print_http_request(const t_httpRequest& request) {
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

void print_http_response(const t_httpResponse& response) {
    // Affichage du statut
    std::cout << COLOR_CYAN << "\n=== HTTP Response ===" << COLOR_RESET << std::endl;
    std::cout << "Status: " << COLOR_YELLOW << response.status << COLOR_RESET;
    switch (response.status) {
        case 200: std::cout << " (OK)"; break;
        case 201: std::cout << " (Created)"; break;
        case 204: std::cout << " (No Content)"; break;
        case 301: std::cout << " (Moved Permanently)"; break;
        case 302: std::cout << " (Found)"; break;
        case 304: std::cout << " (Not Modified)"; break;
        case 400: std::cout << " (Bad Request)"; break;
        case 403: std::cout << " (Forbidden)"; break;
        case 404: std::cout << " (Not Found)"; break;
        case 405: std::cout << " (Method Not Allowed)"; break;
        case 413: std::cout << " (Payload Too Large)"; break;
        case 500: std::cout << " (Internal Server Error)"; break;
        case 501: std::cout << " (Not Implemented)"; break;
        case 502: std::cout << " (Bad Gateway)"; break;
        case 504: std::cout << " (Gateway Timeout)"; break;
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


#endif

