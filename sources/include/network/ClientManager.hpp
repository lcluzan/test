/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ClientManager.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tjacquel <tjacquel@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/25 06:30:07 by bchallat          #+#    #+#             */
/*   Updated: 2026/04/17 15:47:41 by tjacquel         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_MANAGER_HPP
#define CLIENT_MANAGER_HPP

#include <map>
#include <unistd.h>
#include <vector>
#include <string>
#include <iostream>
#include <algorithm>

#include "ClientInfo.hpp"
#include "./../log/colorLog.hpp"

class ClientManager {

  public:
    ClientManager( void );
    virtual ~ClientManager( void );

  public :
    ClientInfo*                       getClient(int fd) const;
    const std::vector<ClientInfo*>&   getClients() const;

  public:
    void                              addClient(int fd, const std::string& ip, int port);
    void                              removeClient(int fd);

  private:
    std::vector<ClientInfo*>          _clients;
    std::map<int, ClientInfo*>        _fileDescriptorClient;

};

# endif

/* ************************************************************************** */
/*                                                                            */
/* ************************************************************************** */

