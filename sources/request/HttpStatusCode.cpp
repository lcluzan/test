/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpStatusCode.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tjacquel <tjacquel@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/19 12:29:21 by bchallat          #+#    #+#             */
/*   Updated: 2026/06/20 05:41:35 by tjacquel         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <request/HttpHandler.hpp>
#include <request/macroHtml.hpp>

/* ========================================================================== */
/*                          -- HEADLER RESPONSSE --                           */
/* ========================================================================== */

/* ================================ ~CODE 2XX ====================================== */

t_httpResponse HttpHandler::status_201(int status, t_httpResponse response)
{
response.body =  "";
response.headers["Content-Length"] = "0";
response.headers["Date"]           = getCurrentHttpDate();
response.headers["Connection"]     = "close";
response.headers["Content-Type"]   = "text/html";

  return (t_httpResponse(status, response.headers, response.body));
}

t_httpResponse HttpHandler::status_204(int status, t_httpResponse response)
{
response.body =  "";
response.headers["Content-Length"] = "0";
response.headers["Date"]           = getCurrentHttpDate();
response.headers["Connection"]     = "close";
response.headers["Content-Type"]   = "text/html";

  return (t_httpResponse(status, response.headers, response.body));
}

/* ================================ ~CODE 3XX ====================================== */

t_httpResponse HttpHandler::status_301(int status, t_httpRequest request, t_httpResponse response, std::string redirURI)
{
  std::ostringstream oss;

  if (response.body.empty()) {

  response.body =  def_301;
  }
  	if (!redirURI.empty() && redirURI[0] != '/') {
		redirURI.insert(0, "/");
	}
  oss << response.body.size();
response.headers["Content-Length"] = oss.str();

response.headers["Date"]           = getCurrentHttpDate();
response.headers["Location"]       = "http://" + request.headers["Host"] + redirURI;
response.headers["Connection"]     = "close";
response.headers["Content-Type"]   = "text/html";

  return (t_httpResponse(status, response.headers, response.body));
}

t_httpResponse HttpHandler::status_302(int status, t_httpRequest request, t_httpResponse response, std::string redirURI)
{
  std::ostringstream oss;

  if (response.body.empty()) {

  response.body =  def_302;
  }
	if (!redirURI.empty() && redirURI[0] != '/') {
		redirURI.insert(0, "/");
	}

  oss << response.body.size();
response.headers["Content-Length"] = oss.str();

response.headers["Date"]           = getCurrentHttpDate();
response.headers["Location"]       = "http://" + request.headers["Host"] + redirURI;
response.headers["Connection"]     = "close";
response.headers["Content-Type"]   = "text/html";

  return (t_httpResponse(status, response.headers, response.body));
}

t_httpResponse HttpHandler::status_304(int status, t_httpRequest request, t_httpResponse response, std::string redirURI)
{
  std::ostringstream oss;

  if (response.body.empty()) {

  response.body =  def_304;
  }
  	if (!redirURI.empty() && redirURI[0] != '/') {
		redirURI.insert(0, "/");
	}
  oss << response.body.size();
response.headers["Content-Length"] = oss.str();

response.headers["Date"]           = getCurrentHttpDate();
response.headers["Location"]       = "http://" + request.headers["Host"] + redirURI;
response.headers["Connection"]     = "close";
response.headers["Content-Type"]   = "text/html";

  return (t_httpResponse(status, response.headers, response.body));
}

/* ================================ ~CODE 4XX ====================================== */

t_httpResponse HttpHandler::status_400(int status, t_httpResponse response)
{
  std::ostringstream oss;

  if (response.body.empty()) {

  response.body =  def_400;
  }
  oss << response.body.size();
response.headers["Content-Length"] = oss.str();

response.headers["Date"]           = getCurrentHttpDate();
response.headers["Connection"]     = "close";
response.headers["Content-Type"]   = "text/html";

  return (t_httpResponse(status, response.headers, response.body));
}

t_httpResponse HttpHandler::status_403(int status, t_httpResponse response)
{
  std::ostringstream oss;

  if (response.body.empty()) {

  response.body =  def_403;
  }
  oss << response.body.size();
response.headers["Content-Length"] = oss.str();

response.headers["Date"]           = getCurrentHttpDate();
response.headers["Connection"]     = "close";
response.headers["Content-Type"]   = "text/html";

  return (t_httpResponse(status, response.headers, response.body));
}

t_httpResponse HttpHandler::status_404(int status, t_httpResponse response)
{
  std::ostringstream oss;

  if (response.body.empty()) {

  response.body =  def_404;
  }
  oss << response.body.size();
response.headers["Content-Length"] = oss.str();

response.headers["Date"]           = getCurrentHttpDate();
response.headers["Connection"]     = "close";
response.headers["Content-Type"]   = "text/html";

  return (t_httpResponse(status, response.headers, response.body));
}

t_httpResponse HttpHandler::status_405(int status, t_httpResponse response, t_config directiv)
{
  std::ostringstream oss;

  if (response.body.empty()) {

  response.body =  def_405;
  }
  oss << response.body.size();
response.headers["Content-Length"] = oss.str();

response.headers["Date"]           = getCurrentHttpDate();
response.headers["Connection"]     = "close";
response.headers["Content-Type"]   = "text/html";

  if ( directiv.location[directiv.prefix].checkMethod("GET"   )) { response.headers["Allow"] += " GET   "; }
  if ( directiv.location[directiv.prefix].checkMethod("POST"  )) { response.headers["Allow"] += " POST  "; }
  if ( directiv.location[directiv.prefix].checkMethod("DELETE")) { response.headers["Allow"] += " DELETE"; }

  return (t_httpResponse(status, response.headers, response.body));
}

t_httpResponse HttpHandler::status_413(int status, t_httpResponse response)
{
  std::ostringstream oss;

  if (response.body.empty()) {

  response.body =  "<html><body><h1>413 Payload Too Large</h1><p>The uploaded file exceeds the maximum allowed size.</p></body></html>";
  }
  oss << response.body.size();
response.headers["Content-Length"] = oss.str();

response.headers["Date"]           = getCurrentHttpDate();
response.headers["Connection"]     = "close";
response.headers["Content-Type"]   = "text/html";

  return (t_httpResponse(status, response.headers, response.body));
}

/* ================================ ~CODE 5XX ====================================== */

t_httpResponse HttpHandler::status_500(int status, t_httpResponse response)
{
  std::ostringstream oss;

  if (response.body.empty()) {

  response.body = "<html><body>500 Internal Server Error</body></html>";
  }
  oss << response.body.size();
response.headers["Content-Length"] = oss.str();

response.headers["Date"]           = getCurrentHttpDate();
response.headers["Connection"]     = "close";
response.headers["Content-Type"]   = "text/html";

  return (t_httpResponse(status, response.headers, response.body));
}

t_httpResponse HttpHandler::status_501(int status, t_httpResponse response)
{
  std::ostringstream oss;

  if (response.body.empty()) {

  response.body = "<html><body>501 Not Implemented</body></html>";
  }
  oss << response.body.size();
response.headers["Content-Length"] = oss.str();

response.headers["Date"]           = getCurrentHttpDate();
response.headers["Connection"]     = "close";
response.headers["Content-Type"]   = "text/html";

  return (t_httpResponse(status, response.headers, response.body));
}
t_httpResponse HttpHandler::status_502(int status, t_httpResponse response)
{
  std::ostringstream oss;

  if (response.body.empty()) {

  response.body =  "<html><body>502 Bad Gateway</body></html>";
  }
  oss << response.body.size();
response.headers["Content-Length"] = oss.str();

response.headers["Date"]           = getCurrentHttpDate();
response.headers["Connection"]     = "close";
response.headers["Content-Type"]   = "text/html";

  return (t_httpResponse(status, response.headers, response.body));
}
t_httpResponse HttpHandler::status_504(int status, t_httpResponse response)
{
  std::ostringstream oss;

  if (response.body.empty()) {

  response.body =  "<html><body>504 Gateway Timeout</body></html>";
  }
  oss << response.body.size();
response.headers["Content-Length"] = oss.str();

response.headers["Date"]           = getCurrentHttpDate();
response.headers["Connection"]     = "close";
response.headers["Content-Type"]   = "text/html";

  return (t_httpResponse(status, response.headers, response.body));
}

/* ========================================================================== */
/*                                                                            */
/* ========================================================================== */
