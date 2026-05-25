/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpHandler.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lcluzan <lcluzan@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/07 13:48:28 by bchallat          #+#    #+#             */
/*   Updated: 2026/05/25 15:14:05 by lcluzan          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <request/HttpHandler.hpp>
#include "../../include/cgi/CgiHandler.hpp"
#include <cstdio>
#include <sys/stat.h> // stat()
#include <dirent.h> // opendir(), readdir()
#include <fstream> // std::ifstream

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

t_httpResponse HttpHandler::setHttpResponse(t_httpRequest request)
{
	t_httpResponse                      response;
  std::map<std::string, std::string>  headers;

	 // 1. Vérifier la méthode
	if (request.method != "GET" && request.method != "POST" && request.method != "HEAD")
	{
    headers["Content-Type"] = "text/html";
    return t_httpResponse(400, headers, "<html><body>400 Bad Request</body></html>");
  }

	// 2. Analyser le chemin
  if (request.path == "/"){
    return serveStaticFile("index.html");
  }if (request.method == "DELETE") {
        return handleDelete(request.path);
  }else if (request.method == "POST" && request.path == "/upload") {
        return handleUpload(request);
    } else if (request.method == "GET") {
        std::string full_path = "./sources/www" + request.path;
        if (isDirectory(full_path)) {
            // On vérifie d'abord si un index.html existe dans ce dossier
            std::string index_path = full_path + "/index.html";
            struct stat buffer;
            if (stat(index_path.c_str(), &buffer) == 0) {
                // Si index.html existe, on triche un peu et on modifie la requête
                // pour qu'elle serve ce fichier normalement juste en dessous
                request.path = request.path + "/index.html";
            } else {
                // S'il n'y a pas d'index.html, on lance notre Autoindex !
                return handleAutoIndex(request.path);
            }
        }
      }else if (isStaticFile(request.path)){
    return serveStaticFile(request.path);
  }else{
    headers["Content-Type"] = "text/html";
    return t_httpResponse(404, headers, "<html>404 Not Found</html>");
  }
  return ( response );
}

/*
static t_httpResponse HandlerErrorHttp(int status)
{
	std::string body = "<html><body><h1>"
					+ std::to_string(status)
					+ " " + getStatusMessage(status)
					+ "</h1></body></html>";

    return t_httpResponse{status, {{"Content-Type", "text/html"}}, body};
}
*/

/* ************************************************************************** */
/*                                                                            */
/* ************************************************************************** */

// static bool isStaticFile(const std::string& path) {
//     if (path.find("/static/") == 0 ||
//         path.substr(path.find_last_of(".") + 1) == "html" ||
//         path.substr(path.find_last_of(".") + 1) == "css" ||
//         path.substr(path.find_last_of(".") + 1) == "js") {
//         return true;
//     }
//     return false;
// }

t_httpResponse HttpHandler::handleDelete(const std::string& path) {
    t_httpResponse response;

    std::string full_path = "./sources/www" + path;

    if (std::remove(full_path.c_str()) == 0) {
      response.status = 204;
      response.body = "";
    } else {
      response.status = 404; // Not Found
      response.body = "<html><body><h1>404 Not Found</h1><p>Le fichier n'existe pas ou ne peut etre supprime.</p></body></html>";
    }
    return response;
}

bool HttpHandler::isDirectory(const std::string& path) {
    struct stat statbuf;

    if (stat(path.c_str(), &statbuf) != 0) {
        return false;
    }
    return S_ISDIR(statbuf.st_mode);
}

t_httpResponse HttpHandler::handleAutoIndex(const std::string& path) {
    t_httpResponse response;
    std::string full_path = "./sources/www" + path;

    // 1. Ouvrir le dossier
    DIR *dir = opendir(full_path.c_str());
    if (dir == NULL) {
        response.status = 403; // Forbidden (pas les droits) ou introuvable
        response.body = "<html><body><h1>403 Forbidden</h1></body></html>";
        return response;
    }

    // 2. Commencer à écrire la page HTML
    std::string html = "<!DOCTYPE html><html><head><title>Index of " + path + "</title></head>";
    html += "<body style='font-family: Arial, sans-serif;'>";
    html += "<h1>Index of " + path + "</h1><hr><ul>";

    // 3. Lire chaque élément du dossier
    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        std::string file_name = entry->d_name;

        // On ignore le dossier actuel "." mais on garde ".." pour pouvoir remonter
        if (file_name == ".") continue;

        // On construit le lien cliquable
        // Si le chemin actuel est "/", on évite de faire "//fichier", on fait "/fichier"
        std::string link = (path == "/") ? "/" + file_name : path + "/" + file_name;

        html += "<li><a href='" + link + "'>" + file_name + "</a></li>";
    }

    // 4. Fermer le dossier et finir le HTML
    closedir(dir);
    html += "</ul><hr></body></html>";

    // 5. Remplir la réponse HTTP
    response.status = 200;
    response.body = html;
    // On précise que c'est du HTML
    response.headers["Content-Type"] = "text/html";

    return response;
}

t_httpResponse HttpHandler::handleUpload(const t_httpRequest& request) {
    t_httpResponse response;

    // 1. Vérifier que c'est bien une requête d'upload (multipart/form-data)
    std::map<std::string, std::string>::const_iterator it = request.headers.find("Content-Type");
    if (it == request.headers.end() || it->second.find("multipart/form-data") == std::string::npos) {
        response.status = 400; // Bad Request
        response.body = "<html><body><h1>400 Bad Request</h1><p>Content-Type invalide pour un upload.</p></body></html>";
        return response;
    }

    // 2. Extraire la "boundary" (la ligne de séparation générée par le navigateur)
    std::string content_type = it->second;
    size_t boundary_pos = content_type.find("boundary=");
    if (boundary_pos == std::string::npos) {
        response.status = 400;
        return response;
    }
    // La boundary dans le body est toujours précédée de "--"
    std::string boundary = "--" + content_type.substr(boundary_pos + 9);

    // 3. Chercher le nom du fichier ("filename") dans le body
    size_t filename_pos = request.body.find("filename=\"");
    if (filename_pos == std::string::npos) {
        response.status = 400;
        response.body = "<html><body><h1>400 Bad Request</h1><p>Aucun fichier trouve dans la requete.</p></body></html>";
        return response;
    }

    // Extraire le nom exact entre les guillemets
    size_t filename_start = filename_pos + 10;
    size_t filename_end = request.body.find("\"", filename_start);
    std::string filename = request.body.substr(filename_start, filename_end - filename_start);

    // 4. Trouver où commencent les vraies données du fichier
    // Les données commencent juste après la séquence "\r\n\r\n" qui suit le filename
    size_t data_start = request.body.find("\r\n\r\n", filename_end);
    if (data_start == std::string::npos) {
        response.status = 400;
        return response;
    }
    data_start += 4; // On avance de 4 caractères pour sauter le "\r\n\r\n"

    // 5. Trouver où finissent les données (au début de la prochaine boundary)
    size_t data_end = request.body.find(boundary, data_start);
    if (data_end == std::string::npos) {
        response.status = 400;
        return response;
    }
    // On recule de 2 caractères pour enlever le "\r\n" qui précède la boundary finale
    data_end -= 2;

    // 6. Extraire le contenu brut du fichier
    std::string file_content = request.body.substr(data_start, data_end - data_start);

    // 7. Sauvegarder le fichier sur le disque
    std::string file_path = "./sources/www/uploads/" + filename;

    // On ouvre un flux en mode binaire (très important pour les images/pdf !)
    std::ofstream outfile(file_path.c_str(), std::ios::out | std::ios::binary);
    if (!outfile.is_open()) {
        response.status = 500; // Internal Server Error
        response.body = "<html><body><h1>500 Erreur Interne</h1><p>Impossible d'enregistrer le fichier.</p></body></html>";
        return response;
    }

    outfile.write(file_content.c_str(), file_content.size());
    outfile.close();

    // 8. Succès ! Le code HTTP officiel pour une création réussie est 201 (Created)
    response.status = 201;
    response.body = "<html><body><h1>201 Created</h1><p>Le fichier " + filename + " a ete uploade avec succes !</p></body></html>";
    return response;
}
