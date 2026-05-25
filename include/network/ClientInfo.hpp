/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ClientInfo.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bchallat <bchallat@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/25 06:30:01 by bchallat          #+#    #+#             */
/*   Updated: 2026/03/25 06:40:59 by bchallat         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_INFO_HPP
#define CLIENT_INFO_HPP

#include <string>

class ClientInfo {

  public:
    ClientInfo(int fd, const std::string& ip, int port);
    virtual ~ClientInfo(void);
  
  public:
    int                 getFileDescriptor(void) const;
    int                 getPortNumber(void) const;
    const std::string&  getAdressIp(void) const;

  private:
    int         _fileDescriptor;
    int         _portNumber;
    std::string _adressIp;
};

#endif

/* ************************************************************************** */
/*                                                                            */
/* ************************************************************************** */

