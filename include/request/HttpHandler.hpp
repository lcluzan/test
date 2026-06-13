/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpHandler.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lcluzan <lcluzan@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/22 11:31:58 by bchallat          #+#    #+#             */
/*   Updated: 2026/06/12 15:23:11 by bchallat         ###   ########.fr       */
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
    static t_httpResponse        HandlerErrorHttp(int status, const ServerConfig& config);

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
    static t_httpResponse        serveStaticFile(const std::string& path, const ServerConfig& config);
    static std::string           getCurrentHttpDate() ;
    static t_post_methode        post_parse_header_request(t_httpRequest request);

  private:
    static t_httpResponse executeCgi(const std::string& path, const t_httpRequest& request);
    static std::vector<std::string> buildCgiEnv(const t_httpRequest& request, const std::string& script_path, const std::string& query_string);
    static std::string getCgiInterpreter(const std::string& path);
    static std::vector<char*> stringsToCharPtrs(const std::vector<std::string>& strings);
    static void handleCgiChild(const std::string& path, const t_httpRequest& request, int pipe_in[2], int pipe_out[2]);

};

# endif // !HTTPHANDLER_HPP

/* ************************************************************************** */
/*                                                                            */
/* ************************************************************************** */

