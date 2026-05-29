/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   httpResponse.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tjacquel <tjacquel@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/22 10:28:48 by bchallat          #+#    #+#             */
/*   Updated: 2026/04/25 19:20:09 by tjacquel         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

//#include "../../include/request/utils_HandlerHttp.hpp"
#include <request/HttpHandler.hpp>
#include <cstdlib>

// Constructeur par défaut
t_httpResponse::t_httpResponse()
    : status(200), body("") {
    headers.insert(std::make_pair("Content-Type", "text/html"));
}
// Constructeur paramétré
t_httpResponse::t_httpResponse(int status, const std::map<std::string, std::string>& headers, const std::string& body)
    : status(status), headers(headers), body(body) {}

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

/* ************************************************************************** */
/*                                                                            */
/* ************************************************************************** */

