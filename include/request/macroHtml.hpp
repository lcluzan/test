/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   macroHtml.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bchallat <bchallat@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/12 10:04:37 by bchallat          #+#    #+#             */
/*   Updated: 2026/06/12 10:19:50 by bchallat         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# ifndef MACROHTML_HPP
# define MACROHTML_HPP

/*
* ╔════════════════════════════════════════════════════════════╗
* ║                                                            ║
* ║                         CODE HTTP 3XX                      ║
* ║                                                            ║
* ╚════════════════════════════════════════════════════════════╝
*/

const char* def_301 = 
    "<!DOCTYPE html>\n"
    "<html>\n"
    "<head>\n"
    "    <title>301 Moved Permanently</title>\n"
    "    <style>\n"
    "        body { font-family: Arial, sans-serif; text-align: center; margin-top: 50px; }\n"
    "        h1 { color: #ff0000; }\n"
    "    </style>\n"
    "</head>\n"
    "<body>\n"
    "    <h1>301 - Moved Permanently</h1>\n"
    "</body>\n"
    "</html>\n";

const char* def_302 = 
    "<!DOCTYPE html>\n"
    "<html>\n"
    "<head>\n"
    "    <title>302 Found</title>\n"
    "    <style>\n"
    "        body { font-family: Arial, sans-serif; text-align: center; margin-top: 50px; }\n"
    "        h1 { color: #ff0000; }\n"
    "    </style>\n"
    "</head>\n"
    "<body>\n"
    "    <h1>302 - Found</h1>\n"
    "</body>\n"
    "</html>\n";


const char* def_304 = 
    "<!DOCTYPE html>\n"
    "<html>\n"
    "<head>\n"
    "    <title>304 Not Modified</title>\n"
    "    <style>\n"
    "        body { font-family: Arial, sans-serif; text-align: center; margin-top: 50px; }\n"
    "        h1 { color: #ff0000; }\n"
    "    </style>\n"
    "</head>\n"
    "<body>\n"
    "    <h1>304 - Not Modified</h1>\n"
    "</body>\n"
    "</html>\n";


/*
* ╔════════════════════════════════════════════════════════════╗
* ║                                                            ║
* ║                         CODE HTTP 4XX                      ║
* ║                                                            ║
* ╚════════════════════════════════════════════════════════════╝
*/

const char* def_400 = 
    "<!DOCTYPE html>\n"
    "<html>\n"
    "<head>\n"
    "    <title>400 Bad Request</title>\n"
    "    <style>\n"
    "        body { font-family: Arial, sans-serif; text-align: center; margin-top: 50px; }\n"
    "        h1 { color: #ff0000; }\n"
    "    </style>\n"
    "</head>\n"
    "<body>\n"
    "    <h1>400 - Bad Request</h1>\n"
    "</body>\n"
    "</html>\n";

const char* def_403 = 
    "<!DOCTYPE html>\n"
    "<html>\n"
    "<head>\n"
    "    <title>403 Forbidden</title>\n"
    "    <style>\n"
    "        body { font-family: Arial, sans-serif; text-align: center; margin-top: 50px; }\n"
    "        h1 { color: #ff0000; }\n"
    "    </style>\n"
    "</head>\n"
    "<body>\n"
    "    <h1>403 - Forbidden</h1>\n"
    "</body>\n"
    "</html>\n";


const char* def_404 = 
    "<!DOCTYPE html>\n"
    "<html>\n"
    "<head>\n"
    "    <title>404 Not Found</title>\n"
    "    <style>\n"
    "        body { font-family: Arial, sans-serif; text-align: center; margin-top: 50px; }\n"
    "        h1 { color: #ff0000; }\n"
    "    </style>\n"
    "</head>\n"
    "<body>\n"
    "    <h1>404 - Page non trouv&eacute;e</h1>\n"
    "    <p>D&eacute;sol&eacute;, la page que vous recherchez n'existe pas.</p>\n"
    "</body>\n"
    "</html>\n";

const char* def_405 = 
    "<!DOCTYPE html>\n"
    "<html>\n"
    "<head>\n"
    "    <title>405 Method Not Allowed</title>\n"
    "    <style>\n"
    "        body { font-family: Arial, sans-serif; text-align: center; margin-top: 50px; }\n"
    "        h1 { color: #ff0000; }\n"
    "    </style>\n"
    "</head>\n"
    "<body>\n"
    "    <h1>405 - Method Not Allowed</h1>\n"
    "</body>\n"
    "</html>\n";




# endif // MACROHTML_HPP

/* ************************************************************************** */
/*                                                                            */
/* ************************************************************************** */

