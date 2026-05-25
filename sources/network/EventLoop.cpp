/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   EventLoop.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lcluzan <lcluzan@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/25 06:01:32 by bchallat          #+#    #+#             */
/*   Updated: 2026/05/25 19:40:08 by lcluzan          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <cstring>  // Pour memset
#include <fcntl.h>
#include <sys/wait.h>

#include "../../include/network/EventLoop.hpp"
#include "../../include/cgi/CgiHandler.hpp"

#include <network/EventLoop.hpp>

EventLoop::EventLoop(const std::vector<int>& ports) : _ports(ports) {}

EventLoop::~EventLoop() { cleanup(); }

/* ************************************************************************** */

void EventLoop::setupServerSockets()
{
  size_t index = 0;
  _server_fds = _socket_handler.createSocketWithPort(_ports);

  while ( index < _server_fds.size())
  {
    struct pollfd pfd;
    pfd.fd = _server_fds[index];
    pfd.events = POLLIN;
    _poll_fds.push_back(pfd);
    index++;
  }
}

/* ************************************************************************** */

int EventLoop::waitForActivity() {
  int ret = poll(&_poll_fds[0], _poll_fds.size(), -1);
  if (ret > 0) {
      // Dès que poll s'arrête, on traite les events CGI
      processCgiEvents();
  }
  return ret;
}

std::vector<int> EventLoop::getActiveFds() const
{
  size_t            index = 0;
  std::vector<int>  active;

  while( index < _poll_fds.size())
  {
    if (_poll_fds[index].revents & POLLIN)
    {
      //On s'assure que le FD n'est pas un pipe de CGI
      if (_cgi_contexts.find(_poll_fds[index].fd) == _cgi_contexts.end())
      {
        active.push_back(_poll_fds[index].fd);
      }
    }
    index++;
  }

  return ( active );
}


bool EventLoop::isServerFd(int fd) const
{
  for (size_t index = 0; index < _server_fds.size(); index++)
  {
    if (_server_fds[index] == fd)
    {
      return( true );
    }
  }

  return( false );
}

/* ************************************************************************** */

ClientInfo* EventLoop::acceptNewConnection(int server_fd)
{
    std::string client_ip;
    int client_port;
    int client_fd = _socket_handler.acceptConnection(server_fd, client_ip, client_port);

    if (client_fd == -1)
    {
        return( NULL );
    }

    _client_manager.addClient(client_fd, client_ip, client_port);

    struct pollfd client_pollfd;
    client_pollfd.fd = client_fd;
    client_pollfd.events = POLLIN;
    _poll_fds.push_back(client_pollfd);

    std::cout << COLOR_CYAN << "(Log Client) : new connection accepted (fd=" << client_fd
              << ", IP=" << client_ip << ", Port=" << client_port << ")" << COLOR_RESET << std::endl;

    return _client_manager.getClient(client_fd);
}

/* ************************************************************************** */

ssize_t EventLoop::readFromClient(int fd, char* buffer, size_t size)
{
  ssize_t bytes_read = 0;
  ClientInfo* client = _client_manager.getClient(fd);
  if (!client)
  {
    std::cerr << "Client non trouvé (fd=" << fd << ")." << std::endl;
    _socket_handler.closeSocket(fd);
    return( -1 );
  }

  memset(buffer, 0, size);
  bytes_read = _socket_handler.readFromSocket(fd, buffer, size - 1);

  if (bytes_read > 0)
  {
    std::cout << COLOR_BLUE << "Données reçues de " << client->getAdressIp() << ":"
              << client->getPortNumber() << " (" << bytes_read << " octets)" << COLOR_RESET << std::endl;
  }

  return bytes_read;
}

/* ************************************************************************** */

void EventLoop::sendResponse(int fd, const std::string& response)
{
  _socket_handler.writeToSocket(fd, response.c_str(), response.size());
}

void EventLoop::removeClient(int fd)
{
  _client_manager.removeClient(fd);
  for (size_t i = 0; i < _poll_fds.size(); i++)
  {
    if (_poll_fds[i].fd == fd)
    {
      _poll_fds.erase(_poll_fds.begin() + i);
      break;
    }
  }
}

void EventLoop::cleanup()
{
  for (size_t i = 0; i < _server_fds.size(); i++)
  {
    _socket_handler.closeSocket(_server_fds[i]);
  }
  const std::vector<ClientInfo*>& clients = _client_manager.getClients();
  for (size_t i = 0; i < clients.size(); i++)
  {
    _socket_handler.closeSocket(clients[i]->getFileDescriptor());
  }
}

void EventLoop::startCgi(int client_fd, const std::string& path, const t_httpRequest& request) {
    // On lance le script PHP en arrière-plan via CgiHandler
    CgiContext* ctx = CgiHandler::startCgi(path, request, client_fd);

    if (!ctx) {
        // En cas d'échec du fork/pipe
        t_httpResponse response(500, std::map<std::string, std::string>(), "500 Internal Error");
        sendResponse(client_fd, response.toString());
        return;
    }
    // On stocke le contexte
    _cgi_contexts[ctx->pipe_in] = ctx;
    _cgi_contexts[ctx->pipe_out] = ctx;

    // On dit à poll() de surveiller l'écriture vers le PHP
    struct pollfd pfd_in;
    pfd_in.fd = ctx->pipe_in;
    pfd_in.events = POLLOUT;
    _poll_fds.push_back(pfd_in);

    // On dit à poll() de surveiller la lecture depuis le PHP
    struct pollfd pfd_out;
    pfd_out.fd = ctx->pipe_out;
    pfd_out.events = POLLIN;
    _poll_fds.push_back(pfd_out);
}

void EventLoop::processCgiEvents() {
    for (size_t i = 0; i < _poll_fds.size(); i++) {
        int current_fd = _poll_fds[i].fd;

        // Est-ce que ce FD est lié à un CGI en cours ?
        if (_cgi_contexts.find(current_fd) != _cgi_contexts.end()) {
            CgiContext* ctx = _cgi_contexts[current_fd];

            // 1. Envoyer le Body (POST) vers le script
            if ((_poll_fds[i].revents & POLLOUT) && current_fd == ctx->pipe_in) {
                handleCgiWrite(ctx, i);
            }
            // 2. Lire le résultat généré par le script
            else if ((_poll_fds[i].revents & POLLIN) && current_fd == ctx->pipe_out) {
                handleCgiRead(ctx, i);
            }
        }
    }
}

void EventLoop::handleCgiWrite(CgiContext* ctx, size_t& index) {
    if (ctx->bytes_written < ctx->request_body.size()) {
        ssize_t ret = write(ctx->pipe_in,
                            ctx->request_body.c_str() + ctx->bytes_written,
                            ctx->request_body.size() - ctx->bytes_written);
        if (ret > 0) {
            ctx->bytes_written += ret;
        }
    }

    // Si on a tout écrit, on ferme l'entrée
    if (ctx->bytes_written >= ctx->request_body.size()) {
        close(ctx->pipe_in);
        _cgi_contexts.erase(ctx->pipe_in);
        _poll_fds.erase(_poll_fds.begin() + index);
        index--; // Ajuste l'index de la boucle
    }
}

void EventLoop::handleCgiRead(CgiContext* ctx, size_t& index) {
    char buffer[4096];
    ssize_t bytes = read(ctx->pipe_out, buffer, sizeof(buffer) - 1);

    if (bytes > 0) {
        buffer[bytes] = '\0';
        ctx->response_buffer += buffer;
    }
    else if (bytes == 0 || (bytes < 0 && errno != EAGAIN)) {
        // Le CGI a fini d'écrire ou a crashé, on finalise la réponse
        finalizeCgiResponse(ctx, index);
    }
}

void EventLoop::finalizeCgiResponse(CgiContext* ctx, size_t& index) {
    // Le script a fini, on nettoie le processus zombie
    waitpid(ctx->pid, NULL, WNOHANG);

    t_httpResponse response;
    size_t header_end = ctx->response_buffer.find("\r\n\r\n");

    if (header_end != std::string::npos) {
        response.status = 200;

        // Parse les headers générés par le CGI
        std::string cgi_headers = ctx->response_buffer.substr(0, header_end);
        size_t line_start = 0;
        size_t line_end = cgi_headers.find("\r\n");

        while (line_end != std::string::npos) {
            std::string line = cgi_headers.substr(line_start, line_end - line_start);
            size_t colon_pos = line.find(":");
            if (colon_pos != std::string::npos) {
                std::string key = line.substr(0, colon_pos);
                size_t val_start = line.find_first_not_of(" ", colon_pos + 1);
                std::string val = (val_start != std::string::npos) ? line.substr(val_start) : "";

                if (key == "Status") response.status = std::atoi(val.c_str());
                else response.headers[key] = val;
            }
            line_start = line_end + 2;
            line_end = cgi_headers.find("\r\n", line_start);
        }

        // Assigne le Body
        response.body = ctx->response_buffer.substr(header_end + 4);
    } else {
        response.status = 502; // Bad Gateway
        response.body = "<html><body>502 Bad Gateway</body></html>";
    }

    // On envoie la réponse au client
    sendResponse(ctx->client_fd, response.toString());

    // Nettoyage complet des descripteurs et de la mémoire
    close(ctx->pipe_out);
    _cgi_contexts.erase(ctx->pipe_out);
    _poll_fds.erase(_poll_fds.begin() + index);
    delete ctx;
    index--; // Ajuste l'index de la boucle
}
