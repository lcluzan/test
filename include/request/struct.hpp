/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   struct.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bchallat <bchallat@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/27 13:55:25 by bchallat          #+#    #+#             */
/*   Updated: 2026/04/27 13:56:15 by bchallat         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef UTILS_HANDLERHTTP_HPP
#define UTILS_HANDLERHTTP_HPP

/* ========================================================================== */
/*                           -- parsing structure  --                         */
/* ========================================================================== */

enum t_type {

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

struct t_token {

	t_type       type;
	std::string  value;
  t_token(t_type t, const std::string& v) : type(t), value(v) {}
  ~t_token(void){};

};

/* ========================================================================== */
/*                    -- struct for Http request/response  --                 */
/* ========================================================================== */

struct t_httpRequest {

	std::string                        method;
  std::string                        path;
  std::string                        version;
  std::map<std::string, std::string> headers;
  std::string                        body;

};

struct t_httpResponse {

	int                                status;
  std::map<std::string, std::string> headers;
	std::string                        body;

  t_httpResponse();
  t_httpResponse(int status, const std::map<std::string, std::string>& headers, const std::string& body);
  std::string toString() const;

};

/* ========================================================================== */
/*                           -- parsing structure  --                         */
/* ========================================================================== */

struct t_post_methode {
  
  std::string                         bodyLength;
  std::string                         nameFile;
  std::string                         boundary;
  std::string                         body;
  std::map<std::string, std::string>  headers;

};

#endif // !UTILS_HANDLERHTTP_HPP

/* ************************************************************************** */
/*                                                                            */
/* ************************************************************************** */

