/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   EventLoop.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lcluzan <lcluzan@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/25 06:29:31 by bchallat          #+#    #+#             */
/*   Updated: 2026/05/25 19:44:09 by lcluzan          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef EVENT_LOOP_HPP
#define EVENT_LOOP_HPP

#include <iostream>
#include <cstring>
#include <vector>
#include <poll.h>
#include <map>


#include <sys/types.h>
#include <network/SocketHandler.hpp>
#include <network/ClientManager.hpp>
#include <log/colorLog.hpp>
#include <request/utils_HandlerHttp.hpp>

struct CgiContext {
    int         client_fd;
    int         pipe_in;
    int         pipe_out;
    pid_t       pid;
    std::string request_body;
    size_t      bytes_written;
    std::string response_buffer;
};

class EventLoop {

  public:
    EventLoop(const std::vector<int>& ports);
    ~EventLoop();

  public:
    void              setupServerSockets();
    void              cleanup();
    void              removeClient(int fd);
    void              sendResponse(int fd, const std::string& response);
    void              startCgi(int client_fd, const std::string& path, const t_httpRequest& request);

  public:
    int               waitForActivity();
    bool              isServerFd(int fd) const;
    ssize_t           readFromClient(int fd, char* buffer, size_t size);
    ClientInfo*       acceptNewConnection(int server_fd);
    std::vector<int>  getActiveFds() const;

  private:
    SocketHandler     _socket_handler;
    ClientManager     _client_manager;
    std::vector<int>  _server_fds;
    std::vector<int>  _ports;
    std::vector<struct pollfd> _poll_fds;
    std::map<int, CgiContext*> _cgi_contexts;

    void processCgiEvents();
    void handleCgiWrite(CgiContext* ctx, size_t& index);
    void handleCgiRead(CgiContext* ctx, size_t& index);
    void finalizeCgiResponse(CgiContext* ctx, size_t& index);

};

#endif // !EVENT_LOOP_HPP

/* ************************************************************************** */
/*                                                                            */
/* ************************************************************************** */
