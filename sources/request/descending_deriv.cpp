/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   descending_deriv.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bchallat <bchallat@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/08 02:15:38 by bchallat          #+#    #+#             */
/*   Updated: 2026/04/27 14:11:53 by bchallat         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <request/HttpHandler.hpp>

static bool dd_first_line(const std::vector<t_token>& lexer)
{
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

/* ************************************************************************** */
/*                                                                            */
/* ************************************************************************** */

