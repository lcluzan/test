/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   EventLoop.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tjacquel <tjacquel@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/25 06:01:32 by bchallat          #+#    #+#             */
/*   Updated: 2026/06/16 21:05:17 by tjacquel         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

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
    pfd.events = POLLIN; //| POLLOUT;
    _poll_fds.push_back(pfd);
    index++;
  }
}

/* ************************************************************************** */
/*
int EventLoop::waitForActivity()
int EventLoop::waitForActivity()
{
  return (poll(&_poll_fds[0], _poll_fds.size(), -1));
}
*/
/*
std::vector<int> EventLoop::getActiveFds() const
std::vector<int> EventLoop::getActiveFds() const
{
  size_t            index = 0;
  std::vector<int>  active;

  while( index < _poll_fds.size())
  {
    if (_poll_fds[index].revents & (POLLIN | POLLOUT | POLLERR | POLLHUP))
    if (_poll_fds[index].revents & (POLLIN | POLLOUT | POLLERR | POLLHUP))
    {
      active.push_back(_poll_fds[index].fd);
    }
    index++;
  }

  return ( active );
}
*/

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

short EventLoop::getPollEvent(int i) const {

  return ( _poll_fds[i].revents );

}

/* ************************************************************************** */

ClientInfo* EventLoop::acceptNewConnection(int server_fd)
{
    std::string client_ip;
    int client_port;
    int client_fd = _socket_handler.acceptConnection(server_fd, client_ip, client_port);

    if (client_fd == -1) {

        return( NULL );
    }

    _client_manager.addClient(client_fd, client_ip, client_port);

    struct pollfd client_pollfd;
    client_pollfd.fd = client_fd;
    client_pollfd.events = POLLIN; //| POLLOUT;
    _poll_fds.push_back(client_pollfd);

    std::cout << COLOR_CYAN << "(Log Client) : new connection accepted (fd=" << client_fd
              << ", IP=" << client_ip << ", Port client=" << client_port << ")" << COLOR_RESET << std::endl;

    return ( _client_manager.getClient(client_fd) );
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
/*
void EventLoop::sendResponse(int fd, const std::string& response)
{
  return _socket_handler.writeToSocket(fd, response.c_str(), response.size());
}
*/
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
  const std::vector<ClientInfo*>& clients = _client_manager.getClients();
  for (size_t i = 0; i < clients.size(); i++)
  {
    _client_manager.removeClient(clients[i]->getFileDescriptor());
  }
  for (size_t i = 0; i < _server_fds.size(); i++)
  {
    _socket_handler.closeSocket(_server_fds[i]);
  }

}


/* ************************************************************************** */
/* ************************************************************************** */
/* ************************************************************************** */

size_t EventLoop::getPollFdsSize() const {
    return _poll_fds.size();
}

const struct pollfd& EventLoop::getPollFd(size_t index) const {
    return _poll_fds[index];
}

// Garde cette version (celle qui utilise _pendingResponses)
void EventLoop::sendResponse(int fd, const std::string& response) {
    queueResponse(fd, response);
}

void EventLoop::queueResponse(int fd, const std::string& response) {
    _pendingResponses[fd] = response;
    // Active POLLOUT pour ce fd
    for (size_t i = 0; i < _poll_fds.size(); i++) {
        if (_poll_fds[i].fd == fd) {
            _poll_fds[i].events |= POLLOUT;
            break;
        }
    }
}

void EventLoop::handlePendingWrites(int fd) {
    std::map<int, std::string>::iterator it = _pendingResponses.find(fd);
    if (it == _pendingResponses.end()) {
        return;
    }

    ssize_t sent = _socket_handler.writeToSocket(fd, it->second.c_str(), it->second.size());
    if (sent > 0) {
        it->second.erase(0, sent);
        if (it->second.empty()) {
            _pendingResponses.erase(it);
            // Désactive POLLOUT
            for (size_t i = 0; i < _poll_fds.size(); i++) {
                if (_poll_fds[i].fd == fd) {
                    _poll_fds[i].events &= ~POLLOUT;
                    break;
                }
            }
			if (_disconnectAfterWrite.count(fd) > 0 &&  _disconnectAfterWrite[fd]) {
				removeClient(fd);
				_disconnectAfterWrite.erase(fd);
			}
        }
    } else {
        // Erreur : fermer le client
        removeClient(fd);
    }
}

void EventLoop::markClientForDisconnect(int fd) {
	this->_disconnectAfterWrite[fd] = true;
}

std::vector<int> EventLoop::getActiveFds() const {
    std::vector<int> active;
    for (size_t i = 0; i < _poll_fds.size(); i++) {
        if (_poll_fds[i].revents & (POLLIN | POLLOUT | POLLERR | POLLHUP)) {
            active.push_back(_poll_fds[i].fd);
        }
    }
    return active;
}

int EventLoop::waitForActivity() {
    for (size_t i = 0; i < _poll_fds.size(); i++) {
        _poll_fds[i].revents = 0;  // Réinitialise revents
    }
    return poll(&_poll_fds[0], _poll_fds.size(), -1);
}



/* ************************************************************************** */
/* ************************************************************************** */
/* ************************************************************************** */


void EventLoop::appendToClientBuffer(int fd, const std::string& data) {
    _clientBuffers[fd] += data;

    // Si on n'a pas encore trouvé la fin des headers, on cherche \r\n\r\n
    if (!_clientHeadersEnded[fd]) {
        size_t header_end_pos = _clientBuffers[fd].find("\r\n\r\n");
        if (header_end_pos != std::string::npos) {
            _clientHeadersEnded[fd] = true;
            // Extraire Content-Length
            size_t content_length_pos = _clientBuffers[fd].find("Content-Length: ");
            if (content_length_pos != std::string::npos) {
                size_t start = content_length_pos + 16;
                size_t end = _clientBuffers[fd].find("\r\n", start);
                if (end != std::string::npos) {
                    _clientContentLength[fd] = static_cast<size_t>(
                        std::atoi(_clientBuffers[fd].substr(start, end - start).c_str())
                    );
                } else {
                    _clientContentLength[fd] = 0; // Pas de Content-Length
                }
            } else {
                _clientContentLength[fd] = 0; // Pas de Content-Length (ex: GET)
            }
        }
    }
}

bool EventLoop::isRequestComplete(int fd) {
    if (!_clientHeadersEnded[fd]) {
        return false; // Headers pas encore terminés
    }
    // Si Content-Length = 0, la requête est complète (ex: GET)
    if (_clientContentLength[fd] == 0) {
        return true;
    }
    // Sinon, vérifier si on a reçu tout le body
    size_t header_end_pos = _clientBuffers[fd].find("\r\n\r\n");
    if (header_end_pos == std::string::npos) {
        return false;
    }
    size_t body_bytes_read = _clientBuffers[fd].size() - (header_end_pos + 4);
    return (body_bytes_read >= _clientContentLength[fd]);
}

std::string EventLoop::getFullRequest(int fd) {
    std::string request = _clientBuffers[fd];
    // Nettoyer les buffers pour ce client
    _clientBuffers.erase(fd);
    _clientContentLength.erase(fd);
    _clientHeadersEnded.erase(fd);
    return request;
}

// 1. Register the pipe so poll() starts watching it for POLLIN
void EventLoop::registerCgi(int client_fd, const t_httpResponse& response) {

	CgiState	CgiContext(client_fd, response.cgi_pid, response.cgi_read_fd, response.cgi_write_fd, response.body);
	_cgi_contexts[client_fd] = CgiContext; // Save the master state under the client_fd

	// 1. Register the READ pipe (POLLIN)
	struct pollfd pfd_read;
    pfd_read.fd = CgiContext.cgi_read_fd;
    pfd_read.events = POLLIN; // Wait for the CGI script to write something
    pfd_read.revents = 0;
    _poll_fds.push_back(pfd_read);
	_pipe_client_fds[CgiContext.cgi_read_fd] = client_fd; // Helper mapping

	// 2. Register the WRITE pipe (POLLOUT) ONLY if there is a body to send
	if (!CgiContext.input_buffer.empty()) {
		struct pollfd pfd_write;
		pfd_write.fd = CgiContext.cgi_write_fd;
		pfd_write.events = POLLOUT;
		pfd_write.revents = 0;
		_poll_fds.push_back(pfd_write);
		_pipe_client_fds[CgiContext.cgi_write_fd] = client_fd;
	}
	else {
		close(CgiContext.cgi_write_fd);
		_cgi_contexts[client_fd].cgi_write_fd = -1;
	}
}

// 2. Check if a triggered FD is a CGI pipe
bool EventLoop::isCgiFd(int fd) {
    return _pipe_client_fds.find(fd) != _pipe_client_fds.end();
}

void EventLoop::handleCgiWrite(int fd, CgiState& CgiContext) {
	ssize_t sent = write(fd, CgiContext.input_buffer.c_str(), CgiContext.input_buffer.size());
	if (sent > 0) {
		CgiContext.input_buffer.erase(0, sent);
		if (CgiContext.input_buffer.empty()) {
			// We sent the whole POST body!
			close(fd); // Tell CGI we are done sending

			// Remove the write FD from poll vector
			for (size_t i = 0; i <_poll_fds.size(); i++) {
				if (_poll_fds[i].fd == fd) {
					_poll_fds.erase(_poll_fds.begin() + i);
					break;
				}
			}
			_pipe_client_fds.erase(fd); // delete the routing for the write pipe
			CgiContext.cgi_write_fd = -1;
		}
	}

}

// 3. Read the output and finish the response when the script ends
void EventLoop::handleCgiRead(int fd, CgiState& CgiContext) {
    char buffer[4096];
    ssize_t bytes = read(fd, buffer, sizeof(buffer) - 1);

    if (bytes > 0) {
        // The script printed something! Save it in the buffer.
        buffer[bytes] = '\0';
        CgiContext.output_buffer.append(buffer, bytes);
    }
    else if (bytes == 0) {
        // EOF: The CGI script has finished executing!

        // 1. Kill the zombie process
        // int wstatus;
        // waitpid(CgiContext.cgi_pid, &wstatus, 0); // The 0 flag tells the Operating System: "If the child process hasn't fully terminated yet, block and wait until it does."
		waitpid(CgiContext.cgi_pid, NULL, WNOHANG); // The WNOHANG flag tells the OS: "If the child is a zombie, reap it. If it is still doing some background cleanup, return immediately so my server can keep running."

        // 2. PARSE CGI OUTPUT
        std::string raw_output = CgiContext.output_buffer;
        std::string cgi_headers = "";
        std::string cgi_body = raw_output;

        // Search for the empty line (\r\n\r\n) that separates CGI headers from CGI body
        size_t header_end = raw_output.find("\r\n\r\n");
        if (header_end != std::string::npos) {
            cgi_headers = raw_output.substr(0, header_end + 2); // Extract headers
            cgi_body = raw_output.substr(header_end + 4);       // Extract body
        }

        // 3. Construct a strictly formatted HTTP/1.1 Response
        std::stringstream ss;
        ss << "HTTP/1.1 200 OK\r\n";

        // Append headers provided by the CGI script (like Content-Type)
        if (!cgi_headers.empty()) {
            ss << cgi_headers;
        } else {
            ss << "Content-Type: text/plain\r\n"; // Fallback if script provides no headers
        }

        // We MUST provide the exact Content-Length of the body
        ss << "Content-Length: " << cgi_body.size() << "\r\n";
        ss << "\r\n"; // Final empty line indicating end of HTTP headers

        // Append the actual HTML/Text body generated by the script
        ss << cgi_body;

        // 4. Queue the perfectly formatted string for the client to download
        queueResponse(CgiContext.client_fd, ss.str());

        // 5. Cleanup the pipe and remove from poll
        close(fd);

        for (size_t i = 0; i < _poll_fds.size(); i++) {
            if (_poll_fds[i].fd == fd) {
                _poll_fds.erase(_poll_fds.begin() + i);
                break;
            }
        }
		_cgi_contexts.erase(_pipe_client_fds[fd]); // we pass the client_fd value
		_pipe_client_fds.erase(fd);
    }
}

void	EventLoop::handleCgiEvent(int fd, short revents) {
	// 1. Find which client this pipe belongs to
	int	client_fd = _pipe_client_fds[fd];

	// 2. Grab the ONE true state reference
	CgiState& CgiContext = _cgi_contexts[client_fd];

	if ((revents & POLLOUT) && fd == CgiContext.cgi_write_fd) {
		handleCgiWrite(fd, CgiContext);
	}

	if ((revents & POLLIN) && fd == CgiContext.cgi_read_fd) {
		handleCgiRead(fd, CgiContext);
	}
}

void	EventLoop::printCgiState() {
	std::map<int, CgiState>::const_iterator it = _cgi_contexts.begin();

	for (; it != _cgi_contexts.end(); it++) {
		std::stringstream ss;
		ss << "    _cgi_contexts[ " << it->first << "]";
		std::cout << ss << ".client_fd=" << it->second.client_fd << "\n";
		std::cout << ss << ".cgi_pid=" << it->second.cgi_pid << "\n";
		std::cout << ss << ".cgi_read_fd=" << it->second.cgi_read_fd << "\n";
		std::cout << ss << ".cgi_write_fd=" << it->second.cgi_write_fd << "\n";
		std::cout << ss << ".input_buffer=" << it->second.input_buffer << "\n";
		std::cout << ss << ".output_buffer=" << it->second.output_buffer << "\n";
		std::cout << ss << ".start_time=" << it->second.start_time << "\n";
	}
}

void	EventLoop::checkCgiTimeout() {
	time_t		currentTime = std::time(NULL);

	// time(&currentTime);

	// EventLoop::printCgiState();

	// Iterate through _cgi_contexts.
	std::map<int, CgiState>::iterator	it = _cgi_contexts.begin();
	while (it != _cgi_contexts.end()) {
		if (std::difftime(currentTime, it->second.start_time) > 5.0) {
			std::cout << "timediff=" << difftime(currentTime, it->second.start_time);
			std::cout << COLOR_RED << "CGI Timeout: killing PID " << it->second.cgi_pid << COLOR_RESET << std::endl;

			// 1. MURDER THE SCRIPT
			kill(it->second.cgi_pid, SIGKILL); // SIGKILL==9
			waitpid(it->second.cgi_pid, NULL, 0);

			// 2. Close the pipes and remove from poll
			if (it->second.cgi_read_fd != - 1) {
				close(it->second.cgi_read_fd);
				_pipe_client_fds.erase(it->second.cgi_read_fd);
				for (size_t i = 0; i < _poll_fds.size(); i++) {
					// std::map<int, CgiState>::iterator itFind = find(_poll_fds.begin(), _poll_fds.end(), it->second.cgi_read_fd);
					if (_poll_fds[i].fd == it->second.cgi_read_fd) {
						_poll_fds.erase(_poll_fds.begin() + i); // kezaoko
						break ;
					}
				}
			}
			if (it->second.cgi_write_fd != -1) {
				close(it->second.cgi_write_fd);
				_pipe_client_fds.erase(it->second.cgi_write_fd);
				for (size_t i = 0; i < _poll_fds.size(); i++) {
					if (_poll_fds[i].fd == it->second.cgi_write_fd) {
						_poll_fds.erase(_poll_fds.begin() + i);
						break ;
					}
				}
			}

			// 3. Send a 504 Error to the client
			std::string	error_response = "HTTP/1.1 504 Gateway Timeout\r\nConnection: close\r\nContent-Length: 22\r\n\r\n<h1>504 Timeout</h1>";
			queueResponse(it->second.client_fd, error_response);
			markClientForDisconnect(it->second.client_fd);

			// 4. Erase the context and safely increment iterator
			std::map<int, CgiState>::iterator it_to_erase = it;
			it++;
			_cgi_contexts.erase(it_to_erase);

		}
		else {
			it++;
		}
	}
}
