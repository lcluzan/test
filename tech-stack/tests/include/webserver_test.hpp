/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   webserver_test.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bchallat <bchallat@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/10 09:55:38 by bchallat          #+#    #+#             */
/*   Updated: 2026/06/10 10:23:36 by bchallat         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <sys/stat.h>   // Pour stat()
#include <unistd.h>     // Pour access()
#include <cerrno>       // Pour errno
#include <cstring>      // Pour strerror

 
#define DEBUG_FLAG  0
#define LOG_FLAG    0

/* ========================================================================== */
/*                          -- LOG TESTING --                                 */
/* ========================================================================== */


void print_http_request(const t_httpRequest& request);
void print_http_response(const t_httpResponse& response); 

std::string typeToString(t_type type);
printTokens(const std::vector<t_token>& lexer);
print_hash(const std::vector<std::string>& hash);

//t_httpResponse HttpHandler::serveStaticFile(const std::string& path) {



/* ========================================================================== */
/*                                                                            */
/* ========================================================================== */






/* ************************************************************************** */
/*                                                                            */
/* ************************************************************************** */

