/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ClientInfo.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lcluzan <lcluzan@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/23 14:33:18 by bchallat          #+#    #+#             */
/*   Updated: 2026/06/10 13:52:20 by lcluzan          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/network/ClientInfo.hpp"

ClientInfo::ClientInfo(int fd, const std::string& ip, int port)
    : _fileDescriptor(fd), _portNumber(port), _adressIp(ip) {}

ClientInfo::~ClientInfo(void) {}

int ClientInfo::getFileDescriptor(void) const
{
    return _fileDescriptor;
}

int ClientInfo::getPortNumber(void) const
{
    return _portNumber;
}

const std::string& ClientInfo::getAdressIp(void) const
{
    return _adressIp;
}

/* ************************************************************************** */
/*                                                                            */
/* ************************************************************************** */
