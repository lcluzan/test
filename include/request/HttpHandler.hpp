/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpHandler.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tjacquel <tjacquel@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/22 11:31:58 by bchallat          #+#    #+#             */
/*   Updated: 2026/06/08 16:15:56 by tjacquel         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# ifndef HANDLERHTTP_HPP
# define HANDLERHTTP_HPP

#define FRONT_V2 0

#include <iostream>
#include <algorithm>
#include <sstream>
#include <stdexcept>
#include <string>
#include <map>
#include <vector>

#include <log/colorLog.hpp>
#include <request/struct.hpp>
#include <config/LocationConfig.hpp>
#include <config/ServerConfig.hpp>

class HttpHandler {

  public:
		static t_httpRequest         setHttpRequest(const std::string& raw_request);
		static t_httpResponse        setHttpResponse(t_httpRequest request, const ServerConfig& config);
    static t_httpResponse        HandlerErrorHttp(int status);

  private :
    static t_httpResponse        handler_methode_get(t_httpRequest& request, const ServerConfig& config);
    static t_httpResponse        handler_methode_post(t_httpRequest request);
    static t_httpResponse        handler_methode_delete(t_httpRequest& request, const ServerConfig& config);

  private:
    static bool                  descending_deriv(std::vector<t_token>& lexer);
    static t_httpRequest         struct_http_request(std::vector<t_token>& lexer);
    static std::vector<t_token>  lexing(const std::string raw_request);

  private:
    static bool                  isStaticFile(const std::string& path);
    static std::string           readFile(const std::string& path);
    static t_httpResponse        serveStaticFile(const std::string& path);
    static t_post_methode        post_parse_header_request(t_httpRequest request);

  private:
    static t_httpResponse executeCgi(const std::string& path, const t_httpRequest& request);
    static std::vector<std::string> buildCgiEnv(const t_httpRequest& request, const std::string& script_path, const std::string& query_string);
    static std::string getCgiInterpreter(const std::string& path);


};

#if FRONT_V2 
  #define ROOT "tech-stack/www/"
#else
  #define ROOT "sources/www/"
#endif

# endif // !HTTPHANDLER_HPP

/* ************************************************************************** */
/*                                                                            */
/* ************************************************************************** */

