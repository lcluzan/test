/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpHandler.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lcluzan <lcluzan@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/22 11:31:58 by bchallat          #+#    #+#             */
/*   Updated: 2026/06/20 18:01:11 by lcluzan          ###   ########.fr       */
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
#include <unistd.h>
#include <sys/wait.h>
#include <cstring>
#include <cstdlib>

#include <ctime>
#include <iomanip>
#include <sys/stat.h> // Pour stat()
#include <unistd.h> // Pour access()
#include <cerrno>   // Pour errno
#include <dirent.h>   // Pour opendir(), readdir(), closedir()

#include <log/colorLog.hpp>
#include <request/struct.hpp>
#include <config/LocationConfig.hpp>
#include <config/ServerConfig.hpp>

struct t_config {

  LocationConfig                        config;
  std::string                           prefix;
  std::map<std::string, LocationConfig>	location;

};

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
    // static bool                  isStaticFile(const std::string& path);
    static std::string           readFile(const std::string& path);
    static t_httpResponse        serveStaticFile(const std::string& path, t_httpRequest& request, const ServerConfig& config);
    static std::string           getCurrentHttpDate() ;
    static t_post_methode        post_parse_header_request(t_httpRequest request);
    static std::string           find_location(std::map<std::string, LocationConfig>	location, std::string path);
    static std::string           generateAutoIndexHTML(const std::string& fullPath, const std::string& requestPath);
    static t_httpResponse        serveIndex(const std::string& fullPath, const std::string& requestPath, t_httpRequest& request, const ServerConfig& config);

  private:
    static t_httpResponse           executeCgi(const std::string& path, const t_httpRequest& request, const ServerConfig& config, const std::string& interpreter, const LocationConfig& location);
    static std::vector<std::string> buildCgiEnv(const t_httpRequest& request, const std::string& filename, const std::string& script_name, const std::string& query_string, const ServerConfig& config);
    static std::vector<char*>       stringsToCharPtrs(const std::vector<std::string>& strings);
    static void                     handleCgiChild(const std::string& path, const t_httpRequest& request, int pipe_in[2], int pipe_out[2], const ServerConfig& config, const std::string& interpreter, const LocationConfig& location);

    static t_httpResponse           cgiConditionForExecute(const ServerConfig& config, t_httpRequest request, std::string actual_path , std::map<std::string, std::string> cgi_map, std::string extension, LocationConfig current_loc );
    // Cookies
    static std::map<std::string, int> _sessions;

  private:
    static std::string      findAndOpenBody(int status, t_httpRequest request, const ServerConfig& config);
    static void             setStructConfig(t_config& directiv, const ServerConfig& serv, std::string path);
    static t_httpResponse   checkRequestIsLegit(t_VarOfsetResponse& var, t_httpRequest request, t_config& directiv, const ServerConfig& serve);

    static t_httpResponse status_201(int status, t_httpResponse response);
    static t_httpResponse status_204(int status, t_httpResponse response);
    static t_httpResponse status_301(int status, t_httpRequest request, t_httpResponse response, std::string redirURI);
    static t_httpResponse status_302(int status, t_httpRequest request, t_httpResponse response, std::string redirURI);
    static t_httpResponse status_304(int status, t_httpRequest request, t_httpResponse response, std::string redirURI);
    static t_httpResponse status_400(int status, t_httpResponse response);
    static t_httpResponse status_403(int status, t_httpResponse response);
    static t_httpResponse status_404(int status, t_httpResponse response);
    static t_httpResponse status_405(int status, t_httpResponse response, t_config directiv);
    static t_httpResponse status_413(int status, t_httpResponse response);
    static t_httpResponse status_500(int status, t_httpResponse response);
    static t_httpResponse status_501(int status, t_httpResponse response);
    static t_httpResponse status_502(int status, t_httpResponse response);
    static t_httpResponse status_504(int status, t_httpResponse response);

    static std::string unchunkBody(const std::string& chuncked_body);

    // Cookie helpers
    static std::string getCookieValue(const std::string& cookieHeader, const std::string& cookieName);
    static std::string generateSessionId();
    static void        handleSession(const t_httpRequest& request, t_httpResponse& response);
};

# endif // !HTTPHANDLER_HPP

/* ************************************************************************** */
/*                                                                            */
/* ************************************************************************** */

