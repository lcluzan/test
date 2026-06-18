/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   response_HttpHandler.cpp                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tjacquel <tjacquel@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/01 12:20:23 by ton_utilisa       #+#    #+#             */
/*   Updated: 2026/06/16 20:26:55 by tjacquel         ###   ########.fr       */
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
/*
static void   unitt_find_location(std::string& str) {
//    -- fonction of unitaire test find location -- //

  std::cout << COLOR_MAGENTA << "result find fonction: " + str << COLOR_RESET << std::endl;

}
*/

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
/*                 -- SERV STATIC FILE IMPLEMENT FONCTION  --                 */
/* ========================================================================== */
/*
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

	std::cout << COLOR_MAGENTA << "serveStaticFile()path=" << path << COLOR_RESET << std::endl;
    if (!isSafePath(path))
    {
      return (HandlerErrorHttp(403));
    }

    std::ifstream file(path.c_str(), std::ios::binary);
    if (!file.is_open())
    {
      std::cout << COLOR_MAGENTA << path << ":" << COLOR_RESET << std::endl;
      return (HandlerErrorHttp(404));
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();

    t_httpResponse response;
    response.status = 200;
    response.headers["Content-Type"] = getMimeType(path);
    response.body = content;
	response.headers["Connection"] = "keep-alive";

    std::cout << COLOR_MAGENTA << "\n   ~~~~~~ METHODE END  ~~~~~~" << COLOR_RESET << std::endl;
    return (response);
}
*/
/* ========================================================================== */
/*                              -- TOOLS --                                   */
/* ========================================================================== */
/*
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
*/
/* ========================================================================== */
/*
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
*/
/* ========================================================================== */
/*                          -- tools of POST METHODE  --                      */
/* ========================================================================== */
/*

static std::string  post_length_body(t_httpRequest request)
{
  std::string length;

  if (!request.headers["Content-Length"].empty()) {

    length = request.headers["Content-Length"];

  } else {

    std::cerr << COLOR_YELLOW << "[Warning] : don't find length of body" << COLOR_RESET << std::endl;
  }

  return ( length );
}


static std::string  post_boundary(t_httpRequest request)
{
  size_t      pos_boundary = 0;
  std::string boundary;

  pos_boundary = request.headers["Content-Type"].find("boundary=");
  if (pos_boundary != std::string::npos) {

    boundary = request.headers["Content-Type"].substr(pos_boundary + 9);
  } else {

    std::cerr << COLOR_YELLOW << "[Warning] : don't find a boundary in headers" << COLOR_RESET <<std::endl;
  }

  return ( boundary );
}

static std::map<std::string, std::string> post_hastable_header(t_httpRequest request)
{
  std::map<std::string, std::string>  headers;

  for (size_t start = 0, ope_pos = 0, key_pos = 0, end = 0; start + end < request.body.size(); start = end + 1) {

    if ((key_pos = request.body.find("Content-", start)) == std::string::npos)
        return (headers);

    if (( ope_pos = request.body.find(":", key_pos)) == std::string::npos)
        return (headers);

    if (( end = request.body.find("\n", key_pos)) == std::string::npos)
        return (headers);

    std::string key = request.body.substr(key_pos, ope_pos - key_pos);
    std::string value = request.body.substr(ope_pos, end - ope_pos);
    if (key.empty() || value.empty())
      return(headers);

    headers[key] = value;
    //headers.insert(std::pair<std::string, std::string>(key, value));
    std::cout << COLOR_MAGENTA << key << headers[key] << COLOR_RESET << std::endl;

  }

  return ( headers );
}

static std::string  post_name_file( t_post_methode parsing)
{
  std::string   name;

  std::string str = parsing.headers["Content-Disposition"];
  size_t filename_pos = str.find("filename=\"");
  if (filename_pos != std::string::npos)
  {

    size_t start_quote = filename_pos + 10; // 9 = longueur de "filename=\""
    size_t end_quote = str.find("\"", start_quote);
    if (end_quote != std::string::npos)
    {
            name = str.substr(start_quote, end_quote - start_quote);
            std::cout << COLOR_MAGENTA << "Nom du fichier : " << name << COLOR_RESET << std::endl;
    }
    else
    {
      std::cerr << COLOR_YELLOW << "[Warning] : don't find a closing quote in name file " << COLOR_RESET <<std::endl;
    }

  }
  else {
      std::cerr << "Erreur : 'filename=\"' non trouvé dans Content-Type." << std::endl;
  }
  return ( name );
}



static void cleanMultipartBody(t_post_methode* postData) {

    // Vérifier si le boundary est présent
    if (postData->boundary.empty()) {
        // Si ce n'est pas une requête multipart, le body est déjà propre
        std::cout << COLOR_CYAN << "-return 0" << COLOR_RESET << std::endl;
        return;
    }

    // Construire les délimiteurs de boundary
    std::string boundaryStart = "--" + postData->boundary;
    std::string boundaryEnd = boundaryStart + "--";

    // Trouver la première occurrence du boundary
    size_t start = postData->body.find(boundaryStart);
    if (start == std::string::npos) {
      std::cout << COLOR_CYAN << "- return 1" + boundaryStart << start << COLOR_RESET << std::endl;
        return; // Boundary non trouvé
    }

    // Trouver le début du contenu du fichier (après les headers de la partie)
    start = postData->body.find("\r\n\r\n", start);
    if (start == std::string::npos) {
        return; // Headers de la partie non trouvés
    }
    start += 4; // Sauter "\r\n\r\n"

    // Trouver la fin du contenu du fichier (prochain boundary ou boundary de fin)
    size_t end = postData->body.find(boundaryStart, start);
    if (end == std::string::npos) {
        end = postData->body.find(boundaryEnd, start);
        if (end == std::string::npos) {
    std::cout << COLOR_CYAN << "-return 2 "<< COLOR_RESET << std::endl;
            return; // Boundary de fin non trouvé
        }
    }

    // Extraire le contenu du fichier
    std::string fileContent = postData->body.substr(start, end - start);

    // Nettoyer les espaces ou sauts de ligne en trop au début et à la fin
    size_t contentStart = fileContent.find_first_not_of("\r\n");
    if (contentStart != std::string::npos) {
        fileContent = fileContent.substr(contentStart);
    }
    size_t contentEnd = fileContent.find_last_not_of("\r\n");
    if (contentEnd != std::string::npos) {
        fileContent = fileContent.substr(0, contentEnd + 1);
    }

    // Mettre à jour le body avec le contenu nettoyé
    postData->body = fileContent;
}
*/
/* ========================================================================== */
/*
t_post_methode HttpHandler::post_parse_header_request(t_httpRequest request)
{
  t_post_methode  parsing;

  if ((parsing.bodyLength = post_length_body(request)).empty() )
    return ( parsing );

  if ((parsing.boundary = post_boundary(request)).empty() )
    return ( parsing );

  if ((parsing.headers = post_hastable_header(request)).empty())
    return( parsing );

  if ((parsing.nameFile = post_name_file( parsing)).empty())
    return ( parsing );

  parsing.body = request.body;
  cleanMultipartBody(&parsing);
  return (parsing);
}
*/
/* ************************************************************************** */
/*                                                                            */
/* ************************************************************************** */

