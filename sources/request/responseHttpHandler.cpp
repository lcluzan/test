/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   responseHttpHandler.cpp                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tjacquel <tjacquel@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/20 04:00:28 by lcluzan           #+#    #+#             */
/*   Updated: 2026/06/20 05:51:43 by tjacquel         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <request/HttpHandler.hpp>

/* ========================================================================== */
/*         -- CONSTRUCTOR/DESTRUCTOR && METHODE OF STRUCT RESPONSE --         */
/* ========================================================================== */

// Constructeur par défaut
t_httpResponse::t_httpResponse()
    : status(200), body(""), is_cgi(false) {
    headers.insert(std::make_pair("Content-Type", "text/html"));
}
// Constructeur paramétré
t_httpResponse::t_httpResponse(int status, const std::map<std::string, std::string>& headers, const std::string& body)
    : status(status), headers(headers), body(body), is_cgi(false) {}

/* ========================================================================== */

// Méthode pour convertir la réponse en chaîne HTTP brute
std::string t_httpResponse::toString() const {
    // std::string response;
	std::stringstream	ss;

    // Ligne de statut (ex: "HTTP/1.1 200 OK")
	// std::stringstream	ss;
	// ss << "HTTP/1.1 " << status << " ";
	// response += ss.str();
	ss << "HTTP/1.1 " << status << " ";
    // response += "HTTP/1.1 " + to_string(status) + " ";
    switch (status) {
        // case 200: response += "OK"; break;
        // case 400: response += "Bad Request"; break;
        // case 404: response += "Not Found"; break;
        // case 501: response += "Not Implemented"; break;
        // default: response += "Unknown Status"; break;
		case 200: ss <<  "OK"; break;
        case 400: ss <<  "Bad Request"; break;
        case 404: ss <<  "Not Found"; break;
        case 501: ss <<  "Not Implemented"; break;
        default:  ss << "Unknown Status"; break;
    }
	ss << "\r\n";
    // response += "\r\n";

    // En-têtes
    for (std::map<std::string, std::string>::const_iterator it = headers.begin(); it != headers.end(); ++it) {
    // response += it->first + ": " + it->second + "\r\n";
	ss << it->first << ": " << it->second << "\r\n";
}

    // Body (si présent)
    if (!body.empty()) {
		// ss << "Content-Length: " << body.size() << "\r\n";
        // response += "Content-Length: " + std::to_string(body.size()) + "\r\n";
		// response += ss.str();
		ss << "Content-Length: " << body.size() << "\r\n";
    }
    // response += "\r\n";
    // response += body;
	ss << "\r\n" << body;

    // return response;
	return ss.str();
}
/* ========================================================================== */
/*                            -- CURR HTTP DATE --                            */
/* ========================================================================== */

std::string HttpHandler::getCurrentHttpDate()
{
     std::time_t now = std::time(NULL);
    std::tm *gmTime = std::gmtime(&now);

    char buffer[100]; // Buffer pour stocker la date formatée
    strftime(buffer, sizeof(buffer), "%a, %d %b %Y %H:%M:%S GMT", gmTime);
    return std::string(buffer);
}

/* ========================================================================== */

std::string HttpHandler::find_location(std::map<std::string, LocationConfig>	location, std::string path)
{
  std::string bestMatch;
  size_t bestMatchLength = 0;
  std::string loc;

  for (std::map<std::string, LocationConfig>::iterator itr = location.begin(); itr != location.end(); itr++)  {
        loc = itr->first;
        if ( loc != "/" && path.find("/" + loc) == 0)// Vérifie si la location est un préfixe du path
        {
            if (loc.length() > bestMatchLength) {
                bestMatch = loc;
                bestMatchLength = loc.length();
            }
        }
    }
    if (bestMatch == "")
      bestMatch = "/";
    return ( bestMatch );
}

/* ========================================================================== */

void HttpHandler::setStructConfig(t_config& directiv, const ServerConfig& serv, std::string path)
{
  directiv.location = serv.getLocationConfig();
  directiv.prefix = find_location(directiv.location, path);
  directiv.config = directiv.location[directiv.prefix];
}

t_httpResponse HttpHandler::cgiConditionForExecute(const ServerConfig& config, t_httpRequest request, std::string actual_path , std::map<std::string, std::string> cgi_map, std::string extension, LocationConfig current_loc )
{
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
  }
  // If it exists and is readable, we safely execute the CGI
  return HttpHandler::executeCgi(request.path, request, config, interpreter, current_loc);
}

t_httpResponse  HttpHandler::checkRequestIsLegit(t_VarOfsetResponse& var, t_httpRequest request, t_config& directiv, const ServerConfig& serve)
{
  // si ya un query ? on a quand meme besoin d'extraire le path sans le ? et ce quil y a apres
  var.extension = "";
  var.actual_path = request.path;
  var.q_pos = var.actual_path.find('?');
  if (var.q_pos != std::string::npos) {

    var.actual_path = var.actual_path.substr(0, var.q_pos);
  }

  directiv.prefix = find_location(directiv.location, var.actual_path);
  directiv.config = directiv.location[directiv.prefix];

  if (request.headers.empty())  {

    return (HandlerErrorHttp(400, request, serve));

  }
  if (request.method != "GET" &&request.method != "POST" &&request.method != "DELETE")  {

      return (HandlerErrorHttp(501, request, serve));
  }

  if (directiv.config.getRedirect().first == 0 && !directiv.config.checkMethod(request.method))  {

    return (HandlerErrorHttp(405, request, serve));

  }
  if (request.body.size() > serve.getClientMaxBodySize()) {  // Check client max body size

	    return (HandlerErrorHttp(413, request, serve));

  }

  // New dynamic CGI location check
  var.dot_pos = var.actual_path.find_last_of(".");
  if (var.dot_pos != std::string::npos) {

    var.extension = var.actual_path.substr(var.dot_pos);
  }
  return (t_httpResponse(0, var.response.headers, ""));
}

/* ========================================================================== */

std::string   HttpHandler::findAndOpenBody(int status, t_httpRequest request, const ServerConfig& config)
{
  std::string                           body;
  std::map<std::string, LocationConfig>	location = config.getLocationConfig();
  std::string                           prefix = find_location(location, request.path);
  std::map<int, std::string>            page = location[prefix].getErrorPage();


  std::string open = location[prefix].getRoot() + "/" + page[status];
  std::ifstream file(open.c_str(), std::ios::binary);
  std::ostringstream oss;

  if (file.is_open() && !page[status].empty())  {

    std::stringstream buffer;
    buffer << file.rdbuf();
    body = buffer.str();

  }
  return (body);
}

/* ************************************************************************** */
/*                                                                            */
/* ************************************************************************** */

