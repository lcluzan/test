/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpHandler.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lcluzan <lcluzan@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/07 13:48:28 by bchallat          #+#    #+#             */
/*   Updated: 2026/06/12 15:39:00 by bchallat         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <request/HttpHandler.hpp>
#include <request/macroHtml.hpp>

/* ========================================================================== */
/*                          -- PARSING REQUESTE --                            */
/* ========================================================================== */

t_httpRequest  HttpHandler::setHttpRequest(const std::string& raw_request)
{
	t_httpRequest       request;
	std::vector<t_token>   lexer;

	lexer = HttpHandler::lexing(raw_request);
	if (HttpHandler::descending_deriv(lexer))
  {
	  request = HttpHandler::struct_http_request(lexer);
    std::cout << COLOR_GREEN << "Success : request is valid " << COLOR_RESET << std::endl;
  }
  else
  {
    std::cout << COLOR_YELLOW << "Warging : bad request " << COLOR_RESET << std::endl;
  }
  
  return ( request );
}

/* ========================================================================== */
/*                          -- SET RESPONSSE --                               */
/* ========================================================================== */


t_httpResponse HttpHandler::setHttpResponse(t_httpRequest request, const ServerConfig& config)
{
	t_httpResponse                        response;
  std::map<std::string, std::string>    headers;
  std::map<std::string, LocationConfig>	location = config.getLocationConfig();
  
	if (!location["/"].checkMethod(request.method))
	{
    return (HandlerErrorHttp(405, config));
  }
  else if (request.path.find("/cgi-bin/") == 0 || request.path.find(".php") != std::string::npos || request.path.find(".py") != std::string::npos)
  {
    return HttpHandler::executeCgi(request.path, request);
  }
  else if (request.method == "GET")
  {
    return (HttpHandler::handler_methode_get(request, config));
  }
  else if (request.method == "POST")
  {
    return (HttpHandler::handler_methode_post(request, config));
  }
  else if (request.method == "DELETE")
  {
    return (HttpHandler::handler_methode_delete(request, config));
  }
  else
  {
    return (HandlerErrorHttp(400, config));
  }
	return ( response );
}

/* ========================================================================== */
/*                          -- HEADLER RESPONSSE --                           */
/* ========================================================================== */


t_httpResponse HttpHandler::HandlerErrorHttp(int status, const ServerConfig& config)
{
	t_httpResponse                      response;
  std::map<std::string, std::string>  headers;
  std::string body;
  std::map<std::string, LocationConfig>	location = config.getLocationConfig();
  std::map<int, std::string> page = location["/"].getErrorPage();

  headers["Date"] = getCurrentHttpDate();
  headers["Connection"] = "close";
  headers["Content-Type"] = "text/html";
  
  std::string open = location["/"].getRoot() + "/" + page[status];
  std::ifstream file(open.c_str(), std::ios::binary);
  std::ostringstream oss;

  if (file.is_open() && !page[status].empty())
  {
    std::stringstream buffer;
    buffer << file.rdbuf();
    body = buffer.str();

  }

  else if (status == 301)
  {
    body = def_301; 
  }
  else if (status == 302)
    body = def_302; 

  else if (status == 304)
    body = def_304; 

  else if (status == 400)
    body = def_400; 

  else if (status == 403)
    body = def_403; 
    
  else if (status == 404)
    body = def_404; 

  else if (status == 405)
    body = def_405;
  
  else if (status == 500)
    body ="<html><body>500 Internal Server Error</body></html>";
  
  else if (status == 501)
    body ="<html><body>501 Not Implemented</body></html>";

  else if (status == 204)
    body ="";

  oss << body.size();
  headers["Content-Length"] = oss.str();

  return (t_httpResponse(status, headers, body));
}
 
/* ========================================================================== */
/*                          -- HTTP METHODE --                                */
/* ========================================================================== */
/*
t_httpResponse HttpHandler::handler_methode_get(t_httpRequest& request, const ServerConfig& config)
{
  std::map<std::string, LocationConfig>	location = config.getLocationConfig();
      std::cout << COLOR_MAGENTA << "   ~~~~~~ METHODE GET  ~~~~~~\n" << COLOR_RESET << std::endl;
  std::cout << COLOR_MAGENTA << location["/"].getRoot() + location["/"].getIndex() << COLOR_RESET << std::endl;

  if (request.path == "/" && location["/"].getAutoindex())
  {
    return HttpHandler::serveStaticFile(location["/"].getRoot() + location["/"].getIndex());
  }
  else if (HttpHandler::isStaticFile(location["/"].getRoot() + request.path))
  {
    return (HttpHandler::serveStaticFile(location["/"].getRoot() + request.path));
  }
  else
    return (HandlerErrorHttp(404));
}
*/
/* ========================================================================== */
/*
t_httpResponse HttpHandler::handler_methode_post(t_httpRequest request, const ServerConfig& config)
{
  std::string                           link;
  t_post_methode                        parsing;
  t_httpResponse                        response;
  std::map<std::string, LocationConfig>	location;
  
  location = config.getLocationConfig();
  std::cout << COLOR_MAGENTA << "   ~~~~~~ METHODE POST ~~~~~~\n" << COLOR_RESET << std::endl;
  if (request.body.empty() || location.empty()) {

    response.status = 404;

  } else if (request.path.find("/upload/") != std::string::npos) {

    parsing = HttpHandler::post_parse_header_request(request);
    link = location["/"].getRoot() + request.path + parsing.nameFile;

    std::ofstream fichier(link.c_str());
    if (parsing.body.empty() || fichier.is_open())
    {
      fichier << parsing.body;
      fichier.close();
      response.status = 201;
      response.headers = parsing.headers;
      
      std::cout << COLOR_GREEN << "Success : Created file " << location["/"].getRoot() + request.path + parsing.nameFile << COLOR_RESET << std::endl;
      std::cout << COLOR_MAGENTA << "\n   ~~~~~~ METHODE END  ~~~~~~" << COLOR_RESET << std::endl;
      return (response);
    }
    else 
      response.status = 404;
  
  } else {
    
    response.status = 404;
    std::cout << COLOR_YELLOW << "Warnig : Faile Created file " << location["/"].getRoot() + request.path + parsing.nameFile << COLOR_RESET << std::endl;

  }
    response.status = 500;

  std::cout << COLOR_MAGENTA << "\n   ~~~~~~ METHODE END  ~~~~~~" << COLOR_RESET << std::endl;
  return (HandlerErrorHttp(response.status));
}
*/
/* ========================================================================== */
/*
t_httpResponse HttpHandler::handler_methode_delete(t_httpRequest& request, const ServerConfig& config)
{
  int     status = 500;
  std::map<std::string, LocationConfig>	location = config.getLocationConfig();

  std::cout << COLOR_MAGENTA << "   ~~~~~~ METHODE DELETE ~~~~~~\n" << COLOR_RESET << std::endl;
  if(remove((location["/"].getRoot() + request.path).c_str()) == 0) {
    status = 204;
    std::cout << COLOR_GREEN << "Success: " << location["/"].getRoot() + request.path <<" is delete !" << COLOR_RESET << std::endl;
  
  }else {
    status = 404 ; 
    std::cout << COLOR_RED << "Le fichier n'a pas été supprimer a cause qu'il n'existe pas, un droit d'accès refusé,..." << COLOR_RESET << std::endl;
  }
  std::cout << COLOR_MAGENTA << "\n   ~~~~~~  METHODE END  ~~~~~~" << COLOR_RESET << std::endl;
  return (HandlerErrorHttp(status));
}
*/
/* ************************************************************************** */
/*                                                                            */
/* ************************************************************************** */

