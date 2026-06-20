/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   EventLoop.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lcluzan <lcluzan@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/25 06:29:31 by bchallat          #+#    #+#             */
/*   Updated: 2026/06/20 04:08:00 by lcluzan          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef EVENT_LOOP_HPP
#define EVENT_LOOP_HPP

#include <iostream>
#include <cstring>
#include <ctime>
#include <vector>
#include <poll.h>
#include <sys/wait.h>
#include <map>
#include <sstream>

#include <config/ServerConfig.hpp>
#include <network/SocketHandler.hpp>
#include <network/ClientManager.hpp>
#include <request/HttpHandler.hpp>
#include <request/struct.hpp>
#include <log/colorLog.hpp>

class EventLoop {

  public:
    EventLoop(const std::vector<int>& ports);
    ~EventLoop();

  public:
    void              setupServerSockets();
    void              cleanup();
    void              removeClient(int fd);
    void              sendResponse(int fd, const std::string& response);

  public:
    int               waitForActivity();
    bool              isServerFd(int fd) const;
    short             getPollEvent(int i) const;
    ssize_t           readFromClient(int fd, char* buffer, size_t size);
    ClientInfo*       acceptNewConnection(int server_fd);
    std::vector<int>  getActiveFds() const;

	  std::string         getClientIP(int fd) const;

  public:
    void registerCgi(int client_fd, const t_httpResponse& response, const t_httpRequest& request, const ServerConfig& config);
    bool isCgiFd(int fd);
    void handleCgiEvent(int fd, short revents);
    void checkCgiTimeout();
    void printCgiState();

  private:
    struct CgiState {
        int           client_fd;
        pid_t         cgi_pid;
        int           cgi_read_fd;
        int           cgi_write_fd;
        std::string   input_buffer;
        std::string   output_buffer;

        time_t        start_time;
        t_httpRequest request;
        ServerConfig  config;


    CgiState() : client_fd(-1), cgi_pid(-1), cgi_read_fd(-1), cgi_write_fd(-1), start_time(0) { }
    CgiState(int client_fd, pid_t cgi_pid, int read_fd, int write_fd,
      const std::string& input_buffer, const t_httpRequest& req, const ServerConfig& config);
      // : client_fd(client_fd), cgi_pid(cgi_pid), cgi_read_fd(read_fd), cgi_write_fd(write_fd), input_buffer(input_buffer),
      //   output_buffer(""), start_time(std::time(NULL)), request(req), config(config) { }
    };
    std::map<int, CgiState> _cgi_contexts;	// Key is CLIENT_FD
    std::map<int, int>		_pipe_client_fds;	// Key is PIPE_FD, Value is CLIENT_FD

    void handleCgiRead(int fd, CgiState& CgiContext);
    void handleCgiWrite(int fd, CgiState& CgiContext);

  private:
    SocketHandler     _socket_handler;
    ClientManager     _client_manager;
    std::vector<int>  _server_fds;
    std::vector<int>  _ports;
    std::vector<struct pollfd> _poll_fds;

/* ************************************************************************** */
  public:
    size_t getPollFdsSize() const;
    const struct pollfd& getPollFd(size_t index) const;
    void queueResponse(int fd, const std::string& response);
    void handlePendingWrites(int fd);

    void markClientForDisconnect(int fd);

  private:
    std::map<int, std::string> _pendingResponses;  // Buffer des réponses
	std::map<int, bool>	_disconnectAfterWrite;



/* ************************************************************************** */
  private:
    // ...
    std::map<int, std::string> _clientBuffers;  // fd → données accumulées
    std::map<int, size_t> _clientContentLength; // fd → Content-Length attendu
    std::map<int, bool> _clientHeadersEnded;    // fd → headers terminés
public:
    // ...
    void appendToClientBuffer(int fd, const std::string& data);
    bool isRequestComplete(int fd);
    std::string getFullRequest(int fd);                                     //
};

#endif // !EVENT_LOOP_HPP

/* ************************************************************************** */
/*                                                                            */
/* ************************************************************************** */
