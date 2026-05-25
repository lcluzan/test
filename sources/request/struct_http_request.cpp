/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   struct_http_request.cpp                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bchallat <bchallat@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/08 12:24:05 by bchallat          #+#    #+#             */
/*   Updated: 2026/04/27 14:19:29 by bchallat         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <request/HttpHandler.hpp>

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

