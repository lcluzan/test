/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   isStaticFile.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bchallat <bchallat@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/27 14:03:56 by bchallat          #+#    #+#             */
/*   Updated: 2026/04/27 14:06:55 by bchallat         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <request/HttpHandler.hpp>

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

/* ************************************************************************** */
/*                                                                            */
/* ************************************************************************** */


