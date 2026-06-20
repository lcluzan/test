/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpHandler.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tjacquel <tjacquel@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/07 13:48:28 by bchallat          #+#    #+#             */
/*   Updated: 2026/06/19 02:08:35 by tjacquel         ###   ########.fr       */
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
  std::map<std::string, LocationConfig> location = config.getLocationConfig();
  // std::string fullPath = location[prefix].getRoot() + request.path ;
  // LocationConfig current_loc = location[prefix];

  // si ya un query ? on a quand meme besoin d'extraire le path sans le ? et ce quil y a apres
  std::string actual_path = request.path;
  size_t q_pos = actual_path.find('?');
  if (q_pos != std::string::npos) {
    actual_path = actual_path.substr(0, q_pos);
  }

    std::string prefix = find_location(location, actual_path);
    LocationConfig current_loc = location[prefix];


  if (request.headers.empty())
  {
    return (HandlerErrorHttp(400, request, config));
  }
	if (!location[prefix].checkMethod(request.method))
	{
    return (HandlerErrorHttp(405, request, config));
  }


  // Check client max body size
  if (request.body.size() > config.getClientMaxBodySize()) {
	return (HandlerErrorHttp(413, request, config));
  }

  // New dynamic CGI location check
  std::string extension = "";
  size_t dot_pos = actual_path.find_last_of(".");
  if (dot_pos != std::string::npos) {
    extension = actual_path.substr(dot_pos);
  }
  std::map<std::string, std::string> cgi_map = current_loc.getCgiPass();
  if (!extension.empty() && cgi_map.find(extension) != cgi_map.end()) {
    std::string interpreter = cgi_map[extension]; // e.g., "/usr/bin/php-cgi"
    if (!interpreter.empty()) {
      std::string full_script_path = current_loc.getRoot() + actual_path;

      // F_OK checks if the file exists.
      if (access(full_script_path.c_str(), F_OK) !=0) {
        return HandlerErrorHttp(404, request, config);
      }

      // R_OK checks if we have permission to read the file.
      if (access(full_script_path.c_str(), R_OK) != 0) {
        return HandlerErrorHttp(403, request, config);
      }

      // If it exists and is readable, we safely execute the CGI
      return HttpHandler::executeCgi(request.path, request, config, interpreter, current_loc);
    }
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
    return (HandlerErrorHttp(500, request, config));
  }
	return ( response );
}

/* ========================================================================== */
/*                          -- HEADLER RESPONSSE --                           */
/* ========================================================================== */


t_httpResponse HttpHandler::HandlerErrorHttp(int status, t_httpRequest request, const ServerConfig& config)
{
	t_httpResponse                      response;
  std::map<std::string, std::string>  headers;
  std::string                         body;

  std::map<std::string, LocationConfig>	location = config.getLocationConfig();
  std::string prefix = find_location(location, request.path);
  std::map<int, std::string>            page = location[prefix].getErrorPage();
  //std::set<std::string>                 allow = location["/"].getMethods();

  headers["Date"] = getCurrentHttpDate();
  headers["Connection"] = "close";
  headers["Content-Type"] = "text/html";

  std::string open = location[prefix].getRoot() + "/" + page[status];
  std::ifstream file(open.c_str(), std::ios::binary);
  std::ostringstream oss;

  if (file.is_open() && !page[status].empty())
  {
    std::stringstream buffer;
    buffer << file.rdbuf();
    body = buffer.str();

  }
  else if (status == 301) {
    headers["Location"] = "http://" + request.headers["Host"] ;
    body = def_301;
  }
  else if (status == 302) {
    headers["Location"] = "http://" + request.headers["Host"] ;
    body = def_302;
  }
  else if (status == 304) {
    headers["Location"] = "http://" + request.headers["Host"] ;
    body = def_304;
  }
  else if (status == 400)
    body = def_400;

  else if (status == 403)
    body = def_403;

  else if (status == 404)
    body = def_404;

  else if (status == 405) {
    body = def_405;
    if ( location[prefix].checkMethod("GET"   )) { headers["Allow"] += " GET   "; }
    if ( location[prefix].checkMethod("POST"  )) { headers["Allow"] += " POST  "; }
    if ( location[prefix].checkMethod("DELETE")) { headers["Allow"] += " DELETE"; }
  }

  else if (status == 413) {
      body = "<html><body><h1>413 Payload Too Large</h1><p>The uploaded file exceeds the maximum allowed size.</p></body></html>";
  }

  else if (status == 500)
    body ="<html><body>500 Internal Server Error</body></html>";

  else if (status == 501)
    body ="<html><body>501 Not Implemented</body></html>";

  else if (status == 504)
    body = "<html><body>504 Gateway Timeout</body></html>";

  else if (status == 204)
    body ="";
  else if (status == 201)
    body ="";
  oss << body.size();
  headers["Content-Length"] = oss.str();

  return (t_httpResponse(status, headers, body));
}

/* ========================================================================== */
/*                                                                            */
/* ========================================================================== */
