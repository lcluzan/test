/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SocketHandler.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tjacquel <tjacquel@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/25 06:32:48 by bchallat          #+#    #+#             */
/*   Updated: 2026/04/17 15:19:22 by tjacquel         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SOCKET_HANDLER_HPP_INCLUDED
#define SOCKET_HANDLER_HPP_INCLUDED

#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <string>
#include <stdexcept>
#include <cstring>
#include <map>
#include <vector>

#include <log/colorLog.hpp>

class SocketHandler {

  public:
    SocketHandler( void );
    virtual ~SocketHandler( void );

  public:
    int               createServerSocket(int port);
    void              closeSocket(int fd);
    int               acceptConnection(int server_fd, std::string& ip, int& port);
    std::vector<int>  createSocketWithPort(const std::vector<int>& ports);

  public:
    ssize_t           readFromSocket(int fd, char* buffer, size_t size);
    ssize_t           writeToSocket(int fd, const char* buffer, size_t size);
};

#endif/* SOCKET_HANDLER_HPP_INCLUDED */

/* ************************************************************************** */
/*                                                                            */
/* ************************************************************************** */

