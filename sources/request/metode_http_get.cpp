/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   metode_http_get.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tjacquel <tjacquel@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/11 16:27:56 by bchallat          #+#    #+#             */
/*   Updated: 2026/06/19 02:34:07 by tjacquel         ###   ########.fr       */
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
  std::string prefix = find_location(location, request.path);
  std::string fullPath = location[prefix].getRoot() + request.path ;
  std::cout << COLOR_MAGENTA << "full path ? " + fullPath << COLOR_RESET << std::endl;

  // Directory handling
  if (!request.path.empty() && request.path[request.path.size() - 1] == '/')
  {
	std::string indexPath = fullPath + location[prefix].getIndex();

	// 1. Check if the directory ITSELF actually exists
	struct stat info;
	if (stat(fullPath.c_str(), &info) != 0 || !S_ISDIR(info.st_mode)) {
		// The directory does not exist
		return (HandlerErrorHttp(404, request, config));
	}

	// 2. Check for the index file
	if (!location[prefix].getIndex().empty() && access(indexPath.c_str(), F_OK) == 0)
      return (HttpHandler::serveStaticFile(indexPath, request, config));

	// 3. Check autoindex fallback
    else if (location[prefix].getAutoindex())
      return HttpHandler::serveIndex(fullPath /* + prefix */, request, config);
	  // jai retire prefixe sinon en fait un http://localhost:8080/cgi-bin/ renvoie 500 Internal Server Error avec autoindex `off`
	  
	// 4. Access denied 403 fallback
    else
      return (HandlerErrorHttp(403, request, config));
  }
  else if (!HttpHandler::isStaticFile(fullPath))
  {
    return (HandlerErrorHttp(301, request, config));
  }
  else if (HttpHandler::isStaticFile(fullPath))
  {
    return (HttpHandler::serveStaticFile(fullPath, request, config));
  }
  else{
    return (HandlerErrorHttp(404, request, config));
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
    // Implémentez la logique pour déterminer le type
    if (path.find(".html") != std::string::npos) return ( "text/html" );
    if (path.find(".css") != std::string::npos) return ( "text/css" );
    // Ajoutez d'autres types selon les besoins
    return ("text/plain");
}

t_httpResponse HttpHandler::serveStaticFile(const std::string& path, t_httpRequest& request, const ServerConfig& config)
{
    if (!isSafePath(path))
    {
      return (HandlerErrorHttp(403, request, config));
    }
    if (request.path.find("/") == std::string::npos)
    {
      return (HandlerErrorHttp(301, request, config));
    }
    std::ifstream file(path.c_str(), std::ios::binary);
    if (!file.is_open())
    {
      std::cout << COLOR_MAGENTA << path << ":" << COLOR_RESET << std::endl;
      return (HandlerErrorHttp(404, request, config));
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();

    t_httpResponse response;
    response.status = 200;
    response.headers["Content-Type"] = getMimeType(path);
    response.headers["Connection"] = "close";
    response.headers["Date"] = getCurrentHttpDate();
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
        path.substr(path.find_last_of(".") + 1) == "js" ||
        path.find_last_of(".") < path.size())
    {
        return true;
    }
    return false;
}

/* ========================================================================== */
/*                              -- AUTO INDEX --                              */
/* ========================================================================== */

#include <dirent.h>   // Pour opendir(), readdir(), closedir()
#include <sys/stat.h> // Pour stat()
#include <algorithm>  // Pour std::sort
#include <ctime>      // Pour ctime()



t_httpResponse HttpHandler::serveIndex(const std::string& path, t_httpRequest& request, const ServerConfig& config)
{
    t_httpResponse response;
    response.status = 200;
    response.headers["Connection"] = "close";
    response.headers["Date"] = getCurrentHttpDate();
    response.body = HttpHandler::generateAutoIndexHTML(path);
    if (response.body.empty()) {

      return ( HandlerErrorHttp(500, request, config) );
    }
    //response.headers["Content-length"] = response.body.length();

    return ( response );
}

std::string HttpHandler::generateAutoIndexHTML(const std::string& path) {

    DIR* dir = opendir(path.c_str());
    if (!dir) {
        return ""; // Erreur : dossier inaccessible
    }

    std::vector<std::string> entries;
    struct dirent* entry;

    // Lire tous les fichiers/dossiers
    while ((entry = readdir(dir)) != NULL) {
        // Ignorer "." et ".."
        if (std::string(entry->d_name) != "." && std::string(entry->d_name) != "..") {
            entries.push_back(entry->d_name);
        }
    }
    closedir(dir);

    // Trier par ordre alphabétique
    std::sort(entries.begin(), entries.end());

    // Générer le HTML
    std::string html = "<!DOCTYPE html>\n";
    html += "<html>\n";
    html += "<head>\n";
    html += "    <title>Index of " + path + "</title>\n";
    html += "    <style>\n";
    html += "        body { font-family: Arial, sans-serif; margin: 20px; }\n";
    html += "        h1 { color: #333; }\n";
    html += "        table { border-collapse: collapse; width: 100%; }\n";
    html += "        th { background-color: #f2f2f2; text-align: left; padding: 8px; }\n";
    html += "        td { border-bottom: 1px solid #ddd; padding: 8px; }\n";
    html += "        a { text-decoration: none; color: #0066cc; }\n";
    html += "        a:hover { text-decoration: underline; }\n";
    html += "    </style>\n";
    html += "</head>\n";
    html += "<body>\n";
    html += "    <h1>Index of " + path + "</h1>\n";
    html += "    <table>\n";
    html += "        <tr><th>Name</th><th>Size</th><th>Last Modified</th></tr>\n";

    // Ajouter chaque entrée
    for (size_t i = 0; i < entries.size(); i++)
    {
        std::string fullPath = path + "/" + entries[i];
        struct stat fileStat;

        if (stat(fullPath.c_str(), &fileStat) == 0)
        {
            std::string size = "-";

            std::string date = ctime(&fileStat.st_mtime);
            date.erase(date.size() - 1); // Supprimer le saut de ligne de ctime()

            html += "        <tr>\n";
            html += "            <td><a href=\"" + entries[i] + "\">" + entries[i] + "</a></td>\n";
            html += "            <td>" + size + "</td>\n";
            html += "            <td>" + date + "</td>\n";
            html += "        </tr>\n";
        }
    }

    html += "    </table>\n";
    html += "</body>\n";
    html += "</html>\n";

    std::cout << "->" + html + "END" << std::endl;

    return html;
}

/* ========================================================================== */
/*                                                                            */
/* ========================================================================== */
