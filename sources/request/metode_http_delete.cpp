/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   metode_http_delete.cpp                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bchallat <bchallat@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/11 16:34:45 by bchallat          #+#    #+#             */
/*   Updated: 2026/06/12 15:11:07 by bchallat         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <request/HttpHandler.hpp>

/* ========================================================================== */
/*                          -- HTTP METHODE --                                */
/* ========================================================================== */

t_httpResponse HttpHandler::handler_methode_delete(t_httpRequest& request, const ServerConfig& config)
{
  int     status = 500;
  std::map<std::string, LocationConfig>	location = config.getLocationConfig();

  struct stat buffer;
  if (stat((location["/"].getRoot() + request.path).c_str(), &buffer) != 0) {

        status = 404;
        std::cout << COLOR_RED << "Erreur : Le fichier '" << location["/"].getRoot() + request.path << "' n'existe pas." << COLOR_RESET << std::endl;
  }
  else if (access((location["/"].getRoot() + request.path).c_str(), W_OK) != 0) {

        status = 403;
    std::cout << COLOR_RED << "Le fichier n'a pas été supprimer un droit d'accès refusé,..." << COLOR_RESET << std::endl;
  }
  else if(remove((location["/"].getRoot() + request.path).c_str()) == 0) {

    status = 204;
    std::cout << COLOR_GREEN << "Success: " << location["/"].getRoot() + request.path <<" is delete !" << COLOR_RESET << std::endl;
  
  }
  else {
    status = 404 ; 
        std::cout << COLOR_RED << "Erreur : Le fichier '" << location["/"].getRoot() + request.path << "' n'existe pas." << COLOR_RESET << std::endl;
  }
 
  return (HandlerErrorHttp(status, request, config));
}

/* ************************************************************************** */
/*                                                                            */
/* ************************************************************************** */

