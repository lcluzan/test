/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   metode_http_get.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lcluzan <lcluzan@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/11 16:27:56 by bchallat          #+#    #+#             */
/*   Updated: 2026/06/20 03:49:18 by lcluzan          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <request/HttpHandler.hpp>

/* ========================================================================== */
/*                                                                            */
/* ========================================================================== */

t_httpResponse	HttpHandler::handler_methode_get(t_httpRequest& request, const ServerConfig& config) {
	std::map<std::string, LocationConfig> location = config.getLocationConfig();

	std::string actual_path = request.path; // uri
	size_t q_pos = actual_path.find('?');
	if (q_pos != std::string::npos) {
		actual_path = actual_path.substr(0, q_pos);
	}

	std::string prefix = find_location(location, actual_path); // location KEY eg. /cgi-bin
	std::string root = location[prefix].getRoot(); // eg. sources/www

	// std::cout << "actual_path=" << actual_path << std::endl;
	// std::cout << "prefix=" << prefix << std::endl;
	// std::cout << "root=" << root << std::endl;

	// 2. build the exact system path
	std::string fullPath = root;

	// Remove the trailing slash from root if it has one (e.g., "sources/www/" -> "sources/www")
	if (!fullPath.empty() && fullPath[fullPath.length() - 1] == '/') {
		fullPath.erase(fullPath.length() - 1);
	}
	// Ensure actual_path starts with a slash
	if (!actual_path.empty() && actual_path[0] != '/') {
		fullPath += "/";
	}
	fullPath += actual_path; // Result: "sources/www" + "/repo/" = "sources/www/repo/"
	// std::cout << "fullPath=" << fullPath << std::endl;

	// 3. ask the OS what the URI correspond to: file or directory
	struct stat info;
	if (stat(fullPath.c_str(), &info) != 0) {
		// 3.a it does not exist on the hard drive
		return (HandlerErrorHttp(404, request, config));
	}
	if (S_ISDIR(info.st_mode)) {
		// 3.b. is a directory

		// check if the user forgot the trailing slash
		if (actual_path[actual_path.length() - 1] != '/') {
			t_httpResponse redirect_resp = HandlerErrorHttp(301, request, config);
			redirect_resp.headers["Location"] = "http://" + request.headers["Host"] + actual_path + "/";
			return redirect_resp;
		}

		// it has a trailing slash: index/autoindex logic
		std::string indexPath = fullPath;
		if (fullPath[fullPath.length() - 1] != '/')
			indexPath += "/";
		indexPath += location[prefix].getIndex();
		std::cout << "indexPath=" << indexPath << std::endl;

		if (!location[prefix].getIndex().empty() && access(indexPath.c_str(), F_OK) == 0) {
			return (HttpHandler::serveStaticFile(indexPath, request, config));
		}
		else if (location[prefix].getAutoindex()) {
			return (HttpHandler::serveIndex(fullPath, request.path, request, config));
		}
		else {
			return (HandlerErrorHttp(403, request, config));
		}
	}
	else {
		// 3.c it is a file
		return (HttpHandler::serveStaticFile(fullPath, request, config));
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
    if (path.find(".html") != std::string::npos) return ( "text/html" );
    if (path.find(".css") != std::string::npos) return ( "text/css" );

    return ("text/plain");
}

t_httpResponse HttpHandler::serveStaticFile(const std::string& path, t_httpRequest& request, const ServerConfig& config)
{
    t_httpResponse response;

    if (!isSafePath(path))
      return ( HandlerErrorHttp(403, request, config) );

    // if (request.path.find("/") == std::string::npos)
    //   return ( HandlerErrorHttp(301, request, config) );

    std::ifstream file(path.c_str(), std::ios::binary);
    if (!file.is_open())
      return ( HandlerErrorHttp(403, request, config) ); // Exists, but permission denied

    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();

    response.status = 200;
    response.headers["Content-Type"] = getMimeType(path);
    response.headers["Connection"] = "close"; //  why keep alive and not close
    response.headers["Date"] = getCurrentHttpDate();
    response.body = content;

    return ( response );
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
/*                              -- AUTO INDEX --                              */
/* ========================================================================== */

t_httpResponse HttpHandler::serveIndex(const std::string& fullPath, const std::string& requestPath, t_httpRequest& request, const ServerConfig& config)
{
    t_httpResponse response;

    response.status = 200;
    response.headers["Connection"] = "close";
    response.headers["Date"] = getCurrentHttpDate();
    response.body = HttpHandler::generateAutoIndexHTML(fullPath, requestPath);

    if (response.body.empty()) {

      std::cout << COLOR_RED << "Error: faile on serve index" << COLOR_RESET << std::endl;
      return ( HandlerErrorHttp(500, request, config) );
    }
    //response.headers["Content-length"] = response.body.length();

    return ( response );
}

std::string HttpHandler::generateAutoIndexHTML(const std::string& fullPath, const std::string& requestPath) {

    DIR* dir = opendir(fullPath.c_str());
    if (!dir) {
        return ""; // Erreur : dossier inaccessible
    }

    std::vector<std::string> entries;
    struct dirent* entry;

    // Lire tous les fichiers/dossiers && Ignorer "." et ".."
    while ((entry = readdir(dir)) != NULL)
    {
        if (std::string(entry->d_name) != "." && std::string(entry->d_name) != "..")
            entries.push_back(entry->d_name);
    }
    closedir(dir);

    // Trier par ordre alphabétique
    std::sort(entries.begin(), entries.end());

    // Générer le HTML
    std::string html = "<!DOCTYPE html>\n";
    html += "<html>\n";
    html += "<head>\n";
    html += "    <title>Index of " + requestPath + "</title>\n";
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
    html += "    <h1>Index of " + requestPath + "</h1>\n";
    html += "    <table>\n";
    html += "        <tr><th>Name</th><th>Size</th><th>Last Modified</th></tr>\n";

    // Ajouter chaque entrée
    for (size_t i = 0; i < entries.size(); i++)
    {
		std::string itemSystemPath = fullPath;
		if (itemSystemPath[itemSystemPath.length() - 1] != '/') {
			itemSystemPath += "/";
		}
		itemSystemPath += entries[i];

        struct stat fileStat;

        if (stat(itemSystemPath.c_str(), &fileStat) == 0)
        {
			std::string sizeStr;
			std::string displayName = entries[i];
			std::string hrefName = entries[i];

			if (S_ISDIR(fileStat.st_mode)) {
				sizeStr = "-";
                displayName += "/";
                hrefName += "/";
			}
			else {
				std::ostringstream iss;
				iss << fileStat.st_size;
				sizeStr = iss.str();
			}

            std::string date = ctime(&fileStat.st_mtime);
            date.erase(date.size() - 1); // Supprimer le saut de ligne de ctime()

            html += "        <tr>\n";
            html += "            <td><a href=\"" + hrefName + "\">" + displayName + "</a></td>\n";
            html += "            <td>" + sizeStr + "</td>\n";
            html += "            <td>" + date + "</td>\n";
            html += "        </tr>\n";
        }
    }

    html += "    </table>\n";
    html += "</body>\n";
    html += "</html>\n";

    return html;
}

/* ========================================================================== */
/*                                                                            */
/* ========================================================================== */
