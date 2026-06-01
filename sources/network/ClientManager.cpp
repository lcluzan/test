/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ClientManager.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tjacquel <tjacquel@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/23 14:46:05 by bchallat          #+#    #+#             */
/*   Updated: 2026/04/25 16:11:46 by tjacquel         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/network/ClientManager.hpp"

ClientManager::ClientManager(void) {}

ClientManager::~ClientManager(void)
{
  size_t  index = 0;

  while(index < _clients.size())
  {
    delete _clients[index];
    index++;
  }

  _clients.clear();
  _fileDescriptorClient.clear();

}

/* ************************************************************************** */

void ClientManager::addClient(int fd, const std::string& ip, int port)
{
  ClientInfo* client = new ClientInfo(fd, ip, port);
  _clients.push_back(client);
  _fileDescriptorClient[fd] = client;

  std::cout << COLOR_CYAN << "(Log Server) : add new client: fd=" << fd << ", IP=" << ip << ", Port=" << port << COLOR_RESET <<std::endl;
}

void ClientManager::removeClient(int fd)
{
  std::map<int, ClientInfo*>::iterator it = _fileDescriptorClient.find(fd);
  if (it == _fileDescriptorClient.end())
  {
    std::cout << COLOR_YELLOW << "Warnig(Log Server) : don't delete client: fd=" << fd << COLOR_RESET << std::endl;
    return;  // Client non trouvé, rien à faire ![TO DO lever l'execption][war]
  }

  ClientInfo* client = it->second;
  _fileDescriptorClient.erase(it);

  if (close(fd) == -1)
    std::cerr << "close() " << std::endl;
  else
    std::cout << COLOR_CYAN <<"(Log Server) : close client fd=" << fd << COLOR_RESET << std::endl;


  std::vector<ClientInfo*>::iterator vec_it = std::find(_clients.begin(), _clients.end(), client);
  if (vec_it != _clients.end())
  {
    _clients.erase(vec_it);
  }

  delete client;
  std::cout << COLOR_CYAN << "(Log Server) : delete client: fd=" << fd << COLOR_RESET << std::endl;
}

/* ************************************************************************** */

ClientInfo* ClientManager::getClient(int fd) const
{
  std::map<int, ClientInfo*>::const_iterator it = _fileDescriptorClient.find(fd);
  if (it == _fileDescriptorClient.end())
  {
    std::cout << COLOR_YELLOW << "Warning(Log Server) : don't find client: fd=" << fd << COLOR_RESET << std::endl;
    return( NULL );  // Client non trouvé ![TO DO lever une exeption][war]
  }
  return( it->second );
}

const std::vector<ClientInfo*>& ClientManager::getClients(void) const
{
    return _clients;
}

/* ************************************************************************** */
/*                                                                            */
/* ************************************************************************** */

