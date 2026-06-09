/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpHandler.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lcluzan <lcluzan@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/07 13:48:28 by bchallat          #+#    #+#             */
/*   Updated: 2026/06/08 17:08:22 by tjacquel         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <request/HttpHandler.hpp>

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

t_httpResponse HttpHandler::setHttpResponse(t_httpRequest request, const ServerConfig& config)
{
	t_httpResponse                        response;
  std::map<std::string, std::string>    headers;
  std::map<std::string, LocationConfig>	location = config.getLocationConfig();

	//if (request.method != "GET" && request.method != "POST" && request.method != "DELETE")
	if (!location["/"].checkMethod(request.method))
	{
    return (HandlerErrorHttp(400));
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
    return (HttpHandler::handler_methode_post(request));
  }
  else if (request.method == "DELETE")
  {
    return (HttpHandler::handler_methode_delete(request, config));
  }
  else
  {
    return (HandlerErrorHttp(404));
  }
	return ( response );
}

/* ========================================================================== */

t_httpResponse HttpHandler::HandlerErrorHttp(int status)
{
	t_httpResponse                      response;
  std::map<std::string, std::string>  headers;
  std::string body;
  
  headers["Connection"] = "close";
  headers["Content-Type"] = "text/html";
  
  std::ostringstream oss;

  if (status == 400)
    body ="<html><body>400 Bad Request</body></html>";

  else if (status == 403)
    body ="<html><body>403 Forbidden</body></html>";
    
  else if (status == 404)
    return (HttpHandler::serveStaticFile("sources/www/404.html"));

  else if (status == 405)
    body ="<html><body>405 Method Not Allowed</body></html>";
  
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

t_httpResponse HttpHandler::handler_methode_get(t_httpRequest& request, const ServerConfig& config)
{
  std::map<std::string, LocationConfig>	location = config.getLocationConfig();

  if (request.path == "/")
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

/* ========================================================================== */

# include <fstream>
t_httpResponse HttpHandler::handler_methode_post(t_httpRequest request)
{
  std::string                         body;
  t_httpResponse                      response;
  t_post_methode                      parsing;
  std::map<std::string, std::string>  headers;


  std::cout << COLOR_YELLOW << "Warnig : a methode post not implement " << COLOR_RESET << std::endl;
  parsing = HttpHandler::post_parse_header_request(request);

  if (request.path.find("/upload") == 0)
  {
    std::string link = ROOT + request.path + parsing.nameFile;
    std::ofstream fichier(link.c_str());
    

    if (fichier.is_open())
    {

      fichier << request.body;
      fichier.close();
      
      response.status = 201;
      response.headers["Content-Type"] = request.headers["Content-Type"];
      
      std::cout << COLOR_GREEN << "Success : Created file " << ROOT + request.path + parsing.nameFile << COLOR_RESET << std::endl;
      return (response);
    }
  }
  std::cout << COLOR_YELLOW << "Warnig : Faile Created file " << ROOT + request.path + "exple.txt" << COLOR_RESET << std::endl;
  return (HandlerErrorHttp(500));

}

/* ========================================================================== */

t_httpResponse HttpHandler::handler_methode_delete(t_httpRequest& request, const ServerConfig& config)
{
  std::map<std::string, LocationConfig>	location = config.getLocationConfig();

  if(remove((location["/"].getRoot() + request.path).c_str()) == 0) 
  {
    std::cout << COLOR_GREEN << "Success: " << location["/"].getRoot() + request.path <<" is delete !" << COLOR_RESET << std::endl;
    return (HandlerErrorHttp(204));
  }
  else
  {
    std::cout << "Le fichier n'a pas été supprimer a cause qu'il n'existe pas, un droit d'accès refusé,..." << std::endl;
  }  
  return (HandlerErrorHttp(404));
}

/* ************************************************************************** */
/*                                                                            */
/* ************************************************************************** */


