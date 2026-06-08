/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   responseHttp.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ton_utilisateur_42 <ton_email@student.42.  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/01 12:20:23 by ton_utilisate     #+#    #+#             */
/*   Updated: 2026/06/01 12:36:01 by ton_utilisate    ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <fstream>   // Pour std::ifstream
#include <sstream>   // Pour std::stringstream

#include <request/HttpHandler.hpp>

/* ========================================================================== */
/*         -- CONSTRUCTOR/DESTRUCTOR && METHODE OF STRUCT RESPONSE --         */
/* ========================================================================== */

// Constructeur par défaut
t_httpResponse::t_httpResponse()
    : status(200), body("") {
    headers.insert(std::make_pair("Content-Type", "text/html"));
}
// Constructeur paramétré
t_httpResponse::t_httpResponse(int status, const std::map<std::string, std::string>& headers, const std::string& body)
    : status(status), headers(headers), body(body) {}

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
/*                 -- SERV STATIC FILE IMPLEMENT FONCTION  --                 */
/* ========================================================================== */

bool isSafePath(const std::string& path) 
{
  return (path.find("../") == std::string::npos); // Empêche les chemins relatifs (comme "../")
}

std::string getMimeType(const std::string& path) 
{
    // Implémentez la logique pour déterminer le type MIME
    if (path.find(".html") != std::string::npos) return "text/html";
    if (path.find(".css") != std::string::npos) return "text/css";
    // Ajoutez d'autres types MIME selon les besoins
    return ("text/plain");
}

t_httpResponse HttpHandler::serveStaticFile(const std::string& path) 
{
    if (!isSafePath(path))
    {
      return (HandlerErrorHttp(403));
    }

    std::string full_path = ROOT + path; // Assurez-vous que le chemin est correct
    std::ifstream file(full_path.c_str(), std::ios::binary);

    if (!file.is_open())
    {
      return (HandlerErrorHttp(404));
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();

    t_httpResponse response;
    response.status = 200;
    response.headers["Content-Type"] = getMimeType(full_path);
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
        return ""; // Le fichier n'existe pas ou n'a pas les droits de lecture
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
/*                          -- tools of POST METHODE  --                      */
/* ========================================================================== */

// fonction pour parser le body 
//
// fonction pour cree le post 
//
// creation de la struct reponsse

t_post_methode HttpHandler::post_parse_header_request(t_httpRequest request)
{
  size_t          pos_boundary = 0;
  t_post_methode  parsing;
  
  if (!request.headers["Content-Length"].empty()) {

    parsing.bodyLength = request.headers["Content-Length"];
    std::cout << " .bodyLength : " << parsing.bodyLength  << std::endl; 
  }

  pos_boundary = request.headers["Content-Type"].find("boundary=");
  if (pos_boundary != 0) {
    
    parsing.boundary = request.headers["Content-Type"].substr(pos_boundary + 9);
    std::cout << " .boundary : " << parsing.boundary << std::endl; 
  }
  parsing.body = request.body;
  parsing.body.erase(parsing.body.find(parsing.boundary), parsing.boundary.length());
  parsing.body.erase(parsing.body.find(parsing.boundary), parsing.boundary.length());
  std::cout << ".body : " << parsing.body << std::endl;
  return (parsing);
}

/* ************************************************************************** */
/*                                                                            */
/* ************************************************************************** */

