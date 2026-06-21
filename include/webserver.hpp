/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   webserver.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tjacquel <tjacquel@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/25 17:39:42 by bchallat          #+#    #+#             */
/*   Updated: 2026/06/21 14:56:58 by tjacquel         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# ifndef WEBSERVER_HPP
# define WEBSERVER_HPP

#define BUFFER_SIZE 4096  // Taille du buffer pour lire les requêtes

#define PRINT_FULL_REQUEST 0

#include <sys/socket.h>  // Pour les sockets
#include <netinet/in.h>  // Pour sockaddr_in
#include <unistd.h>      // Pour close(), read(), write()
#include <poll.h>        // Pour poll()
#include <fcntl.h>       // Pour fcntl() (non-blocking)
#include <cstring>       // Pour memset, strerror
#include <iostream>      // Pour les logs
#include <arpa/inet.h>   // Pour inet_ntoa (affichage des IPs)
#include <csignal>
#include <cstdlib>

#include <log/colorLog.hpp>
#include <network/SocketHandler.hpp>
#include <network/ClientInfo.hpp>
#include <network/ClientManager.hpp>
#include <network/EventLoop.hpp>
#include <request/HttpHandler.hpp>
# include <config/ServerConfig.hpp>
# include <config/LocationConfig.hpp>

/* ========================================================================== */
/*                    -- definition and prototype --                          */
/* ========================================================================== */

void           signal_handler(int signal);

void           print_http_request(const t_httpRequest& request);
void           print_http_response(const t_httpResponse& response);
void           inf_loop(std::vector<int> ports, const std::vector<ServerConfig>& config);
int            getPortServer(int fd);
void           initVerctorOfPort(std::vector<int>& ports, const std::vector<ServerConfig>& server);
ServerConfig  getConfigForHandler(const std::vector<ServerConfig>& vec_config, int port);

# endif

/* ************************************************************************** */
/*                                                                            */
/* ************************************************************************** */
