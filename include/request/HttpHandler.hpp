/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpHandler.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tjacquel <tjacquel@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/22 11:31:58 by bchallat          #+#    #+#             */
/*   Updated: 2026/06/18 22:53:29 by tjacquel         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# ifndef HANDLERHTTP_HPP
# define HANDLERHTTP_HPP

#include <iostream>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <map>
#include <vector>

#include <ctime>
#include <iomanip>
#include <sstream>
#include <sys/stat.h> // Pour stat()
#include <unistd.h> // Pour access()
#include <cerrno>   // Pour errno

#include <log/colorLog.hpp>
#include <request/struct.hpp>
#include <config/LocationConfig.hpp>
#include <config/ServerConfig.hpp>

class HttpHandler {

  public:
		static t_httpRequest         setHttpRequest(const std::string& raw_request);
		static t_httpResponse        setHttpResponse(t_httpRequest request, const ServerConfig& config);
    static t_httpResponse        HandlerErrorHttp(int status, t_httpRequest request, const ServerConfig& config);

  private :
    static t_httpResponse        handler_methode_get(t_httpRequest& request, const ServerConfig& config);
    static t_httpResponse        handler_methode_post(t_httpRequest request, const ServerConfig& config);
    static t_httpResponse        handler_methode_delete(t_httpRequest& request, const ServerConfig& config);

  private:
    static bool                  descending_deriv(std::vector<t_token>& lexer);
    static t_httpRequest         struct_http_request(std::vector<t_token>& lexer);
    static std::vector<t_token>  lexing(const std::string raw_request);

  private:
    static bool                  isStaticFile(const std::string& path);
    static std::string           readFile(const std::string& path);
    static t_httpResponse        serveStaticFile(const std::string& path, t_httpRequest& request, const ServerConfig& config);
    static std::string           getCurrentHttpDate() ;
    static t_post_methode        post_parse_header_request(t_httpRequest request);
    static std::string           find_location(std::map<std::string, LocationConfig>	location, std::string path);
    static std::string           generateAutoIndexHTML(const std::string& path);
    static t_httpResponse        serveIndex(const std::string& path, t_httpRequest& request, const ServerConfig& config);

  private:
    static t_httpResponse executeCgi(const std::string& path, const t_httpRequest& request, const ServerConfig& config, const std::string& interpreter, const LocationConfig& location);
    static std::vector<std::string> buildCgiEnv(const t_httpRequest& request, const std::string& filename, const std::string& script_name, const std::string& query_string, const ServerConfig& config);
    static std::vector<char*> stringsToCharPtrs(const std::vector<std::string>& strings);
    static void handleCgiChild(const std::string& path, const t_httpRequest& request, int pipe_in[2], int pipe_out[2], const ServerConfig& config, const std::string& interpreter, const LocationConfig& location);
};

# endif // !HTTPHANDLER_HPP

/* ************************************************************************** */
/*                                                                            */
/* ************************************************************************** */

