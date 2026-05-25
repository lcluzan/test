/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils_HandlerHttp.hpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tjacquel <tjacquel@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/07 13:37:08 by bchallat          #+#    #+#             */
/*   Updated: 2026/04/25 16:37:25 by tjacquel         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef UTILS_HANDLERHTTP_HPP
#define UTILS_HANDLERHTTP_HPP

#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <map>
#include <vector>

#include <log/colorLog.hpp>
enum t_type
{
	METHOD,
	PATH,
	VERSION,
  HEADER_KEY,
	HEADER_VALUE,
	COLON,
	CRLF,
	BODY,
	WHITESPACE,
	UNKNOWN

};

struct t_token
{
	t_type       type;
	std::string  value;
  t_token(t_type t, const std::string& v) : type(t), value(v) {}
  ~t_token(void){};

};

struct t_httpRequest
{
	std::string                        method;
  std::string                        path;
  std::string                        version;
  std::map<std::string, std::string> headers;
  std::string                        body;

};

struct t_httpResponse
{
	int                                status;
  std::map<std::string, std::string> headers;
	std::string                        body;

  t_httpResponse();
  t_httpResponse(int status, const std::map<std::string, std::string>& headers, const std::string& body);
  std::string toString() const;

};

#endif // !UTILS_HANDLERHTTP_HPP

/* ************************************************************************** */
/*                                                                            */
/* ************************************************************************** */

