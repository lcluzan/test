/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   metode_http_get.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bchallat <bchallat@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/11 16:27:56 by bchallat          #+#    #+#             */
/*   Updated: 2026/06/12 15:42:31 by bchallat         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <request/HttpHandler.hpp>

/* ========================================================================== */
/*                                                                            */
/* ========================================================================== */

t_httpResponse HttpHandler::handler_methode_get(t_httpRequest& request, const ServerConfig& config)
{
  std::map<std::string, LocationConfig>	location = config.getLocationConfig();
  //std::cout << COLOR_MAGENTA << location["/"].getRoot() + location["/"].getIndex() << COLOR_RESET << std::endl;

  if (request.path == "/"  )
  {
    return HttpHandler::serveStaticFile(location["/"].getRoot() + location["/"].getIndex(), config);
  }
  else if (!HttpHandler::isStaticFile(location["/"].getRoot() + request.path))
  {
    return (HandlerErrorHttp(404, config));
  }
  else if (HttpHandler::isStaticFile(location["/"].getRoot() + request.path))
  {
    return (HttpHandler::serveStaticFile(location["/"].getRoot() + request.path, config));
  }
  else{
    return (HandlerErrorHttp(404, config));
  }
}

/* ========================================================================== */
/*                 -- SERV STATIC FILE IMPLEMENT FONCTION  --                 */
/* ========================================================================== */

bool isSafePath(const std::string& path) 
{
  return (path.find("../") == std::string::npos); // Empêche les chemins relatifs (comme "../")
}

std::string getMimeType(const std::string& path) 
{
    // Implémentez la logique pour déterminer le type MIME
    if (path.find(".html") != std::string::npos) return ( "text/html" );
    if (path.find(".css") != std::string::npos) return ( "text/css" );
    // Ajoutez d'autres types MIME selon les besoins
    return ("text/plain");
}

t_httpResponse HttpHandler::serveStaticFile(const std::string& path, const ServerConfig& config) 
{
    if (!isSafePath(path))
    {
      return (HandlerErrorHttp(403, config));
    }
    
    std::ifstream file(path.c_str(), std::ios::binary);
    if (!file.is_open())
    {
      std::cout << COLOR_MAGENTA << path << ":" << COLOR_RESET << std::endl;
      return (HandlerErrorHttp(404, config));
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();

    t_httpResponse response;
    response.status = 200;
    response.headers["Content-Type"] = getMimeType(path);
    response.body = content;
    
    return (response);
}

/* ========================================================================== */
/*                              -- TOOLS --                                   */
/* ========================================================================== */

std::string HttpHandler::readFile(const std::string& filepath) 
{
    std::ifstream       file(filepath.c_str());
    std::ostringstream  ss;

    if (!file.is_open()) 
    {
        return "";
    }
    ss << file.rdbuf();
    
    return (ss.str());
}

/* ========================================================================== */

bool HttpHandler::isStaticFile(const std::string& path)
{
    if (path.find("/static/") == 0 ||
        path.substr(path.find_last_of(".") + 1) == "html" ||
        path.substr(path.find_last_of(".") + 1) == "css" ||
        path.substr(path.find_last_of(".") + 1) == "js") {
        return true;
    }
    return false;
}

/* ========================================================================== */
/*                                                                            */
/* ========================================================================== */
