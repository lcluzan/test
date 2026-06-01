/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsingHttp.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bchallat <bchallat@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/01 11:55:55 by bchallat          #+#    #+#             */
/*   Updated: 2026/06/01 12:05:21 by ton_utilisate    ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <request/HttpHandler.hpp>

#define RUN_WITH_SERVER 0


/* ========================================================================== */
/*                              -- LEXER --                                   */
/* ========================================================================== */

#if RUN_WITH_SERVER
static std::string typeToString(t_type type)
{
    switch (type) {
        case METHOD: return "METHOD";
        case PATH: return "PATH";
        case VERSION: return "VERSION";
        case HEADER_KEY: return "HEADER_KEY";
        case HEADER_VALUE: return "HEADER_VALUE";
        case COLON: return "COLON";
        case CRLF: return "CRLF";
        case BODY: return "BODY";
        case WHITESPACE: return "WHITESPACE";
        case UNKNOWN: return "UNKNOWN";
        default: return "UNKNOWN";
    }
}

static void printTokens(const std::vector<t_token>& lexer)
{
    for (std::vector<t_token>::const_iterator it = lexer.begin(); it != lexer.end(); ++it)
    {
        const t_token& token = *it;
        std::cout << "Token type: " << typeToString(token.type) << ", value: \"" << token.value << "\"" << std::endl;
    }
}

#endif

/* =========================================================================== */

static void lx_tokenizer(std::vector<t_token>& lexer)
{
  int curr = lexer.size() - 1;
  int i = lexer.at(curr).value.size() - 1;
  if (curr == 0 && (lexer.at(curr).value == "GET" || lexer.at(curr).value == "POST" || lexer.at(curr).value == "DELETE "))

  {
    lexer.at(curr).type = METHOD;
  }
  else if (curr == 2 && !lexer.at(curr).value.empty() && lexer.at(curr).value.at(0) == '/')
  {
    lexer.at(curr).type = PATH;
  }
  else if (curr == 4 && lexer.at(curr).value == "HTTP/1.1")
  {
    lexer.at(curr).type = VERSION;
  }
  else if (curr >= 5 && !lexer.at(curr).value.empty() && lexer.at(curr).value.at(i) == ':')
  {
    lexer.at(curr).type = HEADER_KEY;
  }
  else if (curr >= 7 && lexer.at(curr - 2).type == HEADER_KEY)
  {
    lexer.at(curr).type = HEADER_VALUE;
  }
  else if (curr >= 8 && lexer.at(curr - 1).type == HEADER_VALUE)
  {
    lexer.at(curr).type = CRLF;
  }
  else if (lexer.at(curr - 1).type == CRLF || lexer.at(curr - 1).type == BODY)
  {
    lexer.at(curr).type = BODY;
  }
}

static std::string  lx_value(const std::string raw_request, size_t& index)
{
  std::string str;

  while (index < raw_request.size() && raw_request[index] != ' ' )
  {
    if (raw_request[index] == '\r' && raw_request[index + 1] == '\n')
    {
      index += 2;
      break ;
    }
    str = str + raw_request[index];
      index++;
  }
  return ( str );
}

static std::string lx_header_value(const std::string raw_request, size_t& index)
{
  std::string str;

  while (index < raw_request.size() && raw_request[index] != '\r' && raw_request[index + 1] != '\n')
  {
    str = str + raw_request[index];
    index++;
  }
  return ( str );
}

static void  lx_write_value(const std::string raw_request, size_t& index, std::vector<t_token>& lexer)
{
  std::string str;

  if(raw_request[index] == 32)
  {
    t_token temp(WHITESPACE, " ");
    lexer.push_back(temp);
    index++;
  }
  else if (index > 2 && raw_request[index - 2] == ':')
  {
    str = lx_header_value(raw_request, index);
    t_token temp(UNKNOWN, str);
    lexer.push_back(temp);

  }
  else
  {
    str = lx_value(raw_request, index);
    t_token temp(UNKNOWN, str);
    lexer.push_back(temp);
  }
}

std::vector<t_token> HttpHandler::lexing(const std::string raw_request)
{
  std::vector<t_token> lexer;
  size_t index = 0;

  while (index + 1 < raw_request.size())
  {
    lx_write_value(raw_request, index, lexer);
    lx_tokenizer(lexer);
  }
  
#if RUN_WITH_SERVER
  printTokens(lexer);
#endif  
  return ( lexer );
}

/* ========================================================================== */
/*                        -- DESCENDING_DERIV --                              */
/* ========================================================================== */

static bool dd_first_line(const std::vector<t_token>& lexer)
{
  if (lexer.size() < 5)
    return ( false );
  if (lexer[0].type != METHOD)
    return ( false );
  if (lexer[2].type != PATH)
    return ( false );
  if (lexer[4].type != VERSION)
    return ( false );
  return ( true );
}

static bool dd_header_line(const std::vector<t_token>& lexer)
{
  size_t index = 5;
  
  while (index < lexer.size() && lexer[index].type != BODY)
  {
    if (index + 3 >= lexer.size())
      return (false);
    if(lexer[index].type != HEADER_KEY)
      return (false);
    if(lexer[index + 1].type != WHITESPACE)
      return (false);
    if(lexer[index + 2].type != HEADER_VALUE)
      return (false);
    if(lexer[index + 3].type != CRLF)
      return (false);
    index += 4;
  }
  return ( true );
}

static bool dd_header_body(const std::vector<t_token>& lexer)
{
  size_t index = 0;
  while (index < lexer.size() && lexer[index].type != BODY)
    index++;
  while (index < lexer.size() && lexer[index].type == BODY)
    index++;
  if (index != lexer.size())
    return (false);
  return (true);
}

bool  HttpHandler::descending_deriv(std::vector<t_token>& lexer)
{
  if (!dd_first_line(lexer))
    return ( false );
  if (!dd_header_line(lexer))
    return ( false );
  if (!dd_header_body(lexer))
    return ( false );
  return ( true );
}

/* ========================================================================== */
/*               -- CONSTRUCTOR OF STRUCT HTTP REQUEST --                     */
/* ========================================================================== */

static void   init_hash_table_header(t_httpRequest& request, std::vector<t_token>& lexer)
{
  size_t      index = 0;
  std::string h_key;
  std::string h_value;

  while (index + 1 < lexer.size())
  {
    if (lexer[index].type == HEADER_KEY)
    {
      h_key = lexer[index].value;
    }
    else if (lexer[index].type == HEADER_VALUE && !h_key.empty())
    {
      h_value = lexer[index].value;
      request.headers[h_key] = h_value;
    
      h_key.clear();
      h_value.clear();
    }
    index++;
  }
}

static void   st_write_in_struct(t_httpRequest& request, t_token token)
{
  if (token.type == WHITESPACE || token.type == CRLF)
    return ;
  else if (token.type == METHOD)
    request.method = token.value;
  else if (token.type == PATH)
    request.path = token.value;
  else if (token.type == VERSION)
    request.version = token.value;
  else if (token.type == BODY)
    request.body += token.value;
}

t_httpRequest HttpHandler::struct_http_request(std::vector<t_token>& lexer)
{
  size_t        indx = 0;
  t_httpRequest request;

  while (indx < lexer.size()) 
  {
    st_write_in_struct(request, lexer[indx]);
    indx++;
  }
  
  init_hash_table_header(request, lexer);
  return (request);
}

/* ************************************************************************** */
/*                                                                            */
/* ************************************************************************** */


