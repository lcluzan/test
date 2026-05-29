/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpHandler.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bchallat <bchallat@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/22 11:31:58 by bchallat          #+#    #+#             */
/*   Updated: 2026/04/27 15:06:23 by bchallat         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# ifndef HANDLERHTTP_HPP
# define HANDLERHTTP_HPP

#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <map>
#include <vector>

#include <log/colorLog.hpp>
#include <request/struct.hpp>

class HttpHandler {
	
  public:
		static t_httpRequest         setHttpRequest(const std::string& raw_request);
		static t_httpResponse        setHttpResponse(t_httpRequest request);

  private:
    static bool                  descending_deriv(std::vector<t_token>& lexer);
    static t_httpRequest         struct_http_request(std::vector<t_token>& lexer);
    static std::vector<t_token>  lexing(const std::string raw_request);

  private:
    static bool                   isStaticFile(const std::string& path);
    static std::string           readFile(const std::string& path);
    static t_httpResponse        serveStaticFile(const std::string& path);

  private:
    static t_httpResponse executeCgi(const std::string& path, const t_httpRequest& request);
    static std::vector<std::string> buildCgiEnv(const t_httpRequest& request, const std::string& script_path);
    
  private:
    static t_httpResponse        HandlerErrorHttp(int status);

};

# endif // !HTTPHANDLER_HPP 

/* ************************************************************************** */
/*                                                                            */
/* ************************************************************************** */

