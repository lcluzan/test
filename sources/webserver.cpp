/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   webserver.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lcluzan <lcluzan@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/25 17:24:16 by bchallat          #+#    #+#             */
/*   Updated: 2026/06/09 11:39:20 by lcluzan          ###   ########.fr       */
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
#if DEBUG_FLAG
static void print_http_request(const t_httpRequest& request);
static void print_http_response(const t_httpResponse& response);
#endif
std::string readFullHttpRequest(int fd, EventLoop& loop) ;

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
      std::string string_request;

      string_request = readFullHttpRequest(fd, loop);


      // 👇 Traitement Request HTTP

      t_httpRequest request = HttpHandler::setHttpRequest(string_request.c_str());
      t_httpResponse response = HttpHandler::setHttpResponse(request);

      std::string raw_response = response.toString();
      loop.sendResponse(fd, raw_response);
      if (response.status == 400)
      {
        loop.removeClient(fd);

      }
#if DEBUG_FLAG
      print_http_request(request);
      print_http_response(response);
#endif

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
      std::cerr << COLOR_RED << "Error: poll() failed (interrupted system call?)" << COLOR_RESET << std::endl;
      break;
    }
    else
    {
      handler_conection(loop);
    }
  }
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
  ports.push_back(8081);
  ports.push_back(8082);
}

/* ==================================================================================================*/

#if DEBUG_FLAG
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
#endif

#if DEBUG_FLAG
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
    // Affichage du body (si présent)
    std::cout << COLOR_CYAN << "\n--- Body ---" << COLOR_RESET << std::endl;
    if (response.body.empty()) {
        std::cout << COLOR_RED << "No body." << COLOR_RESET << std::endl;
    } else {
        std::cout << COLOR_BLUE << response.body << COLOR_RESET << std::endl;
    }
    // Affichage de la réponse complète (optionnel)
    /*std::cout << COLOR_CYAN << "\n--- Full Response ---" << COLOR_RESET << std::endl;
    std::cout << COLOR_MAGENTA << response.toString() << COLOR_RESET << std::endl;*/

    std::cout << COLOR_CYAN << "====================\n" << COLOR_RESET << std::endl;
}
#endif

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
