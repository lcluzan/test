/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SocketHandler.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tjacquel <tjacquel@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/23 13:33:39 by bchallat          #+#    #+#             */
/*   Updated: 2026/04/17 15:39:42 by tjacquel         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include <network/SocketHandler.hpp>

SocketHandler::SocketHandler() {}
SocketHandler::~SocketHandler() {}

/* ************************************************************************** */

std::vector<int> SocketHandler::createSocketWithPort(const std::vector<int>& ports)
{
  int               fd  = -1;
  size_t            iterator = 0;  
  std::vector<int>  server_fds;

  while(iterator < ports.size()) 
  {
    fd = createServerSocket(ports[iterator]);
    server_fds.push_back(fd);
    iterator++;
  }

  return server_fds;
}

void SocketHandler::closeSocket(int fd)
{
  if (close(fd) == -1)
    std::cerr << "close() " << strerror(errno) << std::endl;
  else
    std::cout << COLOR_GREEN <<"Success : close socket " << COLOR_RESET << std::endl;
}


/* ************************************************************************** */
/*
static struct sockaddr_in  set_socket_addr(int port)
{
  struct sockaddr_in socket_addr;
  
  memset(&socket_addr, 0, sizeof(socket_addr));
  socket_addr.sin_family = AF_INET;
  socket_addr.sin_addr.s_addr = INADDR_ANY;
  socket_addr.sin_port = htons(port);
  
  return ( socket_addr );
}

int SocketHandler::createServerSocket(int port) 
{
  int opt = 1;
  int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
  struct sockaddr_in socket_addr = set_socket_addr(port);
    
  if (socket_fd == -1)
    throw std::runtime_error("socket() " + std::string(strerror(errno)));
  if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1)
  {
        close(socket_fd);
        throw std::runtime_error("setsockopt(SO_REUSEADDR) " + std::string(strerror(errno)));
  }
  if (bind(socket_fd, (struct sockaddr *)&socket_addr, sizeof(socket_addr)) == -1)
  {
    close(socket_fd);
    throw std::runtime_error("bind() " + std::string(strerror(errno)));
  }

  if (listen(socket_fd, SOMAXCONN) == -1) 
  {
    close(socket_fd);
    throw std::runtime_error("listen() " + std::string(strerror(errno)));
  }

  std::cout << COLOR_GREEN << "Success : open socket to a port: " << port << COLOR_RESET << std::endl;
  return( socket_fd );
}
*/

#include <fcntl.h>

static struct sockaddr_in  set_socket_addr(int port)
{
  struct sockaddr_in socket_addr;
  
  memset(&socket_addr, 0, sizeof(socket_addr));
  socket_addr.sin_family = AF_INET;
  socket_addr.sin_addr.s_addr = INADDR_ANY;
  socket_addr.sin_port = htons(port);
  
  return ( socket_addr );
}

static int setnonblocking(int socketFd)
{
    int flags = fcntl(socketFd, F_GETFL, 0); /*get the current flags*/
    if (flags == -1)
    {
        std::cerr << "fcntl failed to put the socket in non blocking mode" << std::endl;
        return (0);
    }
    return (fcntl(socketFd, F_SETFL, flags | O_NONBLOCK)); /*add non blocking mode to the current flags*/
}

int SocketHandler::createServerSocket(int port) 
{
  int opt = 1;
  int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
  struct sockaddr_in socket_addr = set_socket_addr(port);
    
  if (socket_fd == -1)
    throw std::runtime_error("socket() " + std::string(strerror(errno)));
  if (setnonblocking(socket_fd))
  {
    std::cerr << COLOR_RED << "..." << COLOR_RESET << std::endl;
  }
  if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1)
  {
        close(socket_fd);
        throw std::runtime_error("setsockopt(SO_REUSEADDR) " + std::string(strerror(errno)));
  }
  if (bind(socket_fd, (struct sockaddr *)&socket_addr, sizeof(socket_addr)) == -1)
  {
    close(socket_fd);
    throw std::runtime_error("bind() " + std::string(strerror(errno)));
  }

  if (listen(socket_fd, SOMAXCONN) == -1) 
  {
    close(socket_fd);
    throw std::runtime_error("listen() " + std::string(strerror(errno)));
  }

  std::cout << COLOR_GREEN << "Success : open socket to a port: " << port << COLOR_RESET << std::endl;
  return( socket_fd );
}

/* ************************************************************************** */

int SocketHandler::acceptConnection(int socket_fd, std::string& client_ip, int& client_port)
{
    int                 client_fd = 0;
    char                ip_str[INET_ADDRSTRLEN];
    socklen_t           client_addr_len = 0;
    struct sockaddr_in  client_addr;

    client_addr_len = sizeof(client_addr);
    client_fd = accept(socket_fd, (struct sockaddr *)&client_addr, &client_addr_len);

    if (client_fd == -1)
    {
        throw std::runtime_error("accept() " + std::string(strerror(errno)));
    }
    else
    {
      inet_ntop(AF_INET, &(client_addr.sin_addr), ip_str, INET_ADDRSTRLEN);
      client_ip = std::string(ip_str);
      client_port = ntohs(client_addr.sin_port);
    }

    std::cout << COLOR_GREEN << "Success : new connetion accept to " << client_ip << ":" << client_port << COLOR_RESET << std::endl;
    return( client_fd );
}

/* ************************************************************************** */

ssize_t SocketHandler::readFromSocket(int fd, char* buffer, size_t size)
{
    ssize_t bytes_read = read(fd, buffer, size);

    if (bytes_read == -1)
        throw std::runtime_error("read() " + std::string(strerror(errno)));

    return bytes_read;
}

ssize_t SocketHandler::writeToSocket(int fd, const char* buffer, size_t size)
{
    ssize_t bytes_written = write(fd, buffer, size);

    if (bytes_written == -1)
        throw std::runtime_error("write() " + std::string(strerror(errno)));

    return bytes_written;
}

/* ************************************************************************** */
/*                                                                            */
/* ************************************************************************** */

