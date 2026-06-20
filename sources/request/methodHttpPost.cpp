/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   metode_http_post.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bchallat <bchallat@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/11 16:32:30 by bchallat          #+#    #+#             */
/*   Updated: 2026/06/12 15:12:22 by bchallat         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <request/HttpHandler.hpp>

/* ========================================================================== */
/*                          -- HTTP METHODE --                                */
/* ========================================================================== */

t_httpResponse HttpHandler::handler_methode_post(t_httpRequest request, const ServerConfig& config)
{
  std::string                           link;
  std::string                           body;
  t_post_methode                        parsing;
  t_httpResponse                        response;
  std::map<std::string, LocationConfig>	location;
  
  location = config.getLocationConfig();
  std::string prefix = find_location(location, request.path);
  std::string fullPath = location[prefix].getRoot() + request.path ;
  std::cout << COLOR_MAGENTA << "full path ? " + fullPath << COLOR_RESET << std::endl;

  if (!request.path.empty() && fullPath[request.path.size() - 1] != '/')  { 

      std::cout << COLOR_MAGENTA << " -> " + fullPath << COLOR_RESET << std::endl;
      fullPath += "/"; 
  }
  if (request.body.empty() || location[prefix].getRoot().empty()) {

    response.status = 404;

  }else if (access((location[prefix].getRoot()).c_str(), W_OK) != 0) {

        response.status = 403;
        std::cout << COLOR_RED << "error droit d'accès refusé,..." << COLOR_RESET << std::endl;

  } else if (request.path.find(prefix) != std::string::npos) {

    parsing = HttpHandler::post_parse_header_request(request);
    if (parsing.body.empty() && parsing.nameFile.empty())  {
      
      body = request.body;
      link = fullPath;
    
    } else  {
    
      body = parsing.body;
      link = fullPath + parsing.nameFile;

    }


    std::ofstream fichier(link.c_str());
    if (!parsing.body.empty() || !parsing.nameFile.empty() || fichier.is_open())
    {
      fichier << body;
      fichier.close();
      /*    TO DO  -- delete the gestion of responst struct */ 
      response.status = 201;
      response.headers["Date"] = HttpHandler::getCurrentHttpDate();
      response.headers["Connection"] = "close";
      response.headers["Location"] = "http://" + request.headers["Host"] ;
      response.headers["Content-Type"] = "text/html";

      //response.headers = parsing.headers;
      
      std::cout << COLOR_GREEN << "Success : Created file " << fullPath <<"|"<< parsing.nameFile.empty() << COLOR_RESET << std::endl;
      return (HandlerErrorHttp(response.status, request, config));
    }
    else 
      response.status = 403;
  
  } else {
    
    response.status = 409;
    std::cout << COLOR_YELLOW << "Warnig : Faile Created file " << location["/"].getRoot() + request.path + parsing.nameFile << COLOR_RESET << std::endl;

  }

  return (HandlerErrorHttp(response.status, request, config));
}

/* ========================================================================== */
/*                          -- tools of POST METHODE  --                      */
/* ========================================================================== */


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

/* ========================================================================== */

t_post_methode HttpHandler::post_parse_header_request(t_httpRequest request)
{
  t_post_methode  parsing;
  
  if ((parsing.bodyLength = post_length_body(request)).empty() )
    return ( parsing );
 
  if ((parsing.boundary = post_boundary(request)).empty() )
    return ( parsing );

  if ((parsing.headers = post_hastable_header(request)).empty())
    return( parsing );

  if ((parsing.nameFile = post_name_file(parsing)).empty())
    return ( parsing );

  parsing.body = request.body;
  cleanMultipartBody(&parsing);
  return (parsing);
}

/* ************************************************************************** */
/*                                                                            */
/* ************************************************************************** */

