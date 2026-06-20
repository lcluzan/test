/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   struct.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tjacquel <tjacquel@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/27 13:55:25 by bchallat          #+#    #+#             */
/*   Updated: 2026/06/19 18:47:46 by bchallat         ###   ########.fr       */
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

  std::string						client_ip; // for CGI
};

struct t_httpResponse {

	int                                status;
  std::map<std::string, std::string> headers;
	std::string                        body;

  bool                               is_cgi;
  int                                cgi_read_fd;
  int                                cgi_write_fd;
  pid_t                              cgi_pid;

  t_httpResponse();
  t_httpResponse(int status, const std::map<std::string, std::string>& headers, const std::string& body);
  std::string toString() const;

};

/* ========================================================================== */
/*                       -- structure for setResponse --                      */
/* ========================================================================== */

struct t_setResponseCGI {

  size_t                              dot_pos ;
  std::string                         extension;
  std::string                         interpreter;
  std::string                         full_script_path;
  std::map<std::string, std::string>  cgi_map;

};

struct t_VarOfsetResponse {

  size_t                                q_pos;
  size_t                                dot_pos;
  std::string                           extension;
  std::string                           actual_path;
  t_httpResponse                        response;
  
};

/* ========================================================================== */
/*                   -- structure for HandlerErrorHttp --                     */
/* ========================================================================== */

struct t_ErrorHttp {

  std::string                         body;
  std::string                         prefix;
  std::string                         open;
  t_httpResponse                      response;
  std::map<int, std::string>          page;
  std::map<std::string, std::string>  headers;

};

/* ========================================================================== */
/*                         -- structure parsing POST --                       */
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

