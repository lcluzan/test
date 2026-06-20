/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpHandler.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tjacquel <tjacquel@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/07 13:48:28 by bchallat          #+#    #+#             */
/*   Updated: 2026/06/20 05:36:00 by tjacquel         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <request/HttpHandler.hpp>

/* ========================================================================== */
/*                          -- PARSING REQUESTE --                            */
/* ========================================================================== */

t_httpRequest  HttpHandler::setHttpRequest(const std::string& raw_request)
{
	t_httpRequest       request;
	std::vector<t_token>   lexer;

	lexer = HttpHandler::lexing(raw_request);
	if (HttpHandler::descending_deriv(lexer))
  {
	  request = HttpHandler::struct_http_request(lexer);
    std::cout << COLOR_GREEN << "Success : descending deriv " << COLOR_RESET << std::endl;
  }
  else
  {
    std::cout << COLOR_YELLOW << "Warging : bad request " << COLOR_RESET << std::endl;
  }

  return ( request );
}

/* ========================================================================== */
/*                          -- SET RESPONSSE --                               */
/* ========================================================================== */

t_httpResponse HttpHandler::setHttpResponse(t_httpRequest request, const ServerConfig& config)
{
  t_config            directiv;
  t_httpResponse      response;
  t_VarOfsetResponse  var;

  directiv.location = config.getLocationConfig();
  response = checkRequestIsLegit(var, request, directiv, config);
  if (!response.status == 0) {

    return (response);
  }

  // check if there is a redirection
  std::pair<int, std::string> redir = directiv.config.getRedirect();
  if (redir.first != 0) {
	t_httpResponse redirResp = HttpHandler::HandlerErrorHttp(redir.first, request, config);
	if (redir.second.find("http://") != std::string::npos || redir.second.find("https://") != std::string::npos) {
		redirResp.headers["Location"] = redir.second;
	}
	return (redirResp);
  }

  std::map<std::string, std::string> cgi_map = directiv.config.getCgiPass();
  if (!var.extension.empty() && cgi_map.find(var.extension) != cgi_map.end()) {

    return (cgiConditionForExecute(config, request, var.actual_path, cgi_map, var.extension, directiv.config));

  } else if (request.method == "GET")  {

      return (HttpHandler::handler_methode_get(request, config));

  } else if (request.method == "POST") {

      return (HttpHandler::handler_methode_post(request, config));

  } else if (request.method == "DELETE")  {

      return (HttpHandler::handler_methode_delete(request, config));

  } else  {

      return (HandlerErrorHttp(500, request, config));
  }
	return ( var.response );
}

/* ========================================================================== */
/*                          -- HEADLER RESPONSSE --                           */
/* ========================================================================== */

t_httpResponse HttpHandler::HandlerErrorHttp(int status, t_httpRequest request, const ServerConfig& serv)
{
  t_ErrorHttp   stError;
  t_config      directiv;

  setStructConfig(directiv, serv, request.path);
  stError.response.body  = HttpHandler::findAndOpenBody(status, request, serv);

  switch ( status ) {

    case 201:
      return ( HttpHandler::status_201(status, stError.response) );

    case 204:
      return ( HttpHandler::status_204(status, stError.response) );

    case 301:
      return ( HttpHandler::status_301(status, request, stError.response, directiv.config.getRedirect().second) );

    case 302:
      return ( HttpHandler::status_302(status, request, stError.response, directiv.config.getRedirect().second) );

    case 304:
      return ( HttpHandler::status_304(status, request, stError.response, directiv.config.getRedirect().second) );

    case 400:
      return ( HttpHandler::status_400(status, stError.response) );

    case 403:
      return ( HttpHandler::status_403(status, stError.response) );

    case 404:
      return ( HttpHandler::status_404(status, stError.response) );

    case 405:
      return ( HttpHandler::status_405(status, stError.response, directiv) );

    case 413:
      return ( HttpHandler::status_413(status, stError.response) );

    case 501:
      return ( HttpHandler::status_501(status, stError.response) );

    case 504:
      return ( HttpHandler::status_504(status, stError.response) );
  }

  return ( HttpHandler::status_500(status, stError.response) );

}

/* ========================================================================== */
/*                                                                            */
/* ========================================================================== */
