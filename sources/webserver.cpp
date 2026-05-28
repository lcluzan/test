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
static void initVerctorOfPort(std::vector<int>& ports);
static void signal_handler(int signal);
static void print_http_request(const t_httpRequest& request);


int main()
{
	
	std::signal(SIGINT, signal_handler);// Enregistrer le gestionnaire de signal
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
      print_http_request(request);
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

    // Affichage du body (si présent)
    std::cout << COLOR_CYAN << "\n--- Body ---" << COLOR_RESET << std::endl;
    if (request.body.empty()) {
        std::cout << COLOR_RED << "No body." << COLOR_RESET << std::endl;
    } else {
        std::cout << COLOR_BLUE << request.body << COLOR_RESET << std::endl;
    }

    std::cout << COLOR_CYAN << "===================\n" << COLOR_RESET << std::endl;
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
