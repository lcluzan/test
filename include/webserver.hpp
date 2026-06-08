/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   webserver.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lcluzan <lcluzan@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/25 17:39:42 by bchallat          #+#    #+#             */
/*   Updated: 2026/06/02 11:09:55 by bchallat         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# ifndef WEBSERVER_HPP
# define WEBSERVER_HPP

#define PORT 8080        // Port d'écoute du serveur
#define MAX_CLIENTS 10   // Nombre max de clients simultanés
#define BUFFER_SIZE 4096  // Taille du buffer pour lire les requêtes

#define DEBUG_FLAG 1

#include <sys/socket.h>  // Pour les sockets
#include <netinet/in.h>  // Pour sockaddr_in
#include <unistd.h>      // Pour close(), read(), write()
#include <poll.h>        // Pour poll()
#include <fcntl.h>       // Pour fcntl() (non-blocking)
#include <cstring>       // Pour memset, strerror
#include <iostream>      // Pour les logs
#include <arpa/inet.h>   // Pour inet_ntoa (affichage des IPs)

#include "./log/colorLog.hpp"
#include "./network/SocketHandler.hpp"
#include "./network/ClientInfo.hpp"
#include "./network/ClientManager.hpp"
#include "./network/EventLoop.hpp"
#include <request/HttpHandler.hpp>
# include "./ServerConfig.hpp"
//#include "./request/RequestHttp.hpp"

# endif

/* ************************************************************************** */
/*                                                                            */
/* ************************************************************************** */
