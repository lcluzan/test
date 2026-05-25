/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpHandler.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lcluzan <lcluzan@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/22 11:31:58 by bchallat          #+#    #+#             */
/*   Updated: 2026/05/25 15:12:10 by lcluzan          ###   ########.fr       */
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

/*   public:
		static t_httpRequest         setHttpRequest(const std::string& raw_request);
		static t_httpResponse        setHttpResponse(t_httpRequest request);
 */
  private:
    static bool                  descending_deriv(std::vector<t_token>& lexer);
    static t_httpRequest         struct_http_request(std::vector<t_token>& lexer);
    static std::vector<t_token>  lexing(const std::string raw_request);

  private:
    static bool                   isStaticFile(const std::string& path);
    static void                  print_http_response(const t_httpResponse& response);
    static std::string           readFile(const std::string& path);
    static t_httpResponse        serveStaticFile(const std::string& path);

/*   private:
    static t_httpResponse        HandlerErrorHttp(int status); */

  public:
		static t_httpRequest  setHttpRequest(const std::string& raw_request);
		static t_httpResponse setHttpResponse(t_httpRequest request);
    static t_httpResponse HandlerErrorHttp(int status);

  private:
    static t_httpResponse handleDelete(const std::string& path);
    static bool           isDirectory(const std::string& path);
    static t_httpResponse handleAutoIndex(const std::string& path);
    static t_httpResponse handleUpload(const t_httpRequest& request);
};

# endif // !HTTPHANDLER_HPP

/* ************************************************************************** */
/*                                                                            */
/* ************************************************************************** */

