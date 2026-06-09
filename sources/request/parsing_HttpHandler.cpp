/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing_HttpHandler.cpp                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bchallat <bchallat@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/01 11:55:55 by bchallat          #+#    #+#             */
/*   Updated: 2026/06/02 11:10:16 by bchallat         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <request/HttpHandler.hpp>
#include <webserver.hpp>

/* ========================================================================== */
/*                              -- LEXER --                                   */
/* ========================================================================== */
#if DEBUG_FLAG
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

static void print_hash(const std::vector<std::string>& hash)
{
    for (size_t i = 0; i < hash.size(); ++i)
    {
        std::cout << "Line " << i << ": \"" << hash[i] << "\"" << std::endl;
    }
}
#endif
/* =========================================================================== */

static std::vector<std::string> lx_hash_request(const std::string& raw_request)
{
  size_t start = 0;
  size_t end = 0;
  std::vector<std::string> hash;

  while (start < raw_request.size())
  {
    // Cherche le prochain \r\n
    end = raw_request.find("\r\n", start);

    // Si aucun \r\n n'est trouvé, on ajoute le reste de la chaîne et on sort
    if (end == std::string::npos)
    {
      hash.push_back(raw_request.substr(start));
      return ( hash );
    }

    // Extrait la sous-chaîne entre start et end
    hash.push_back(raw_request.substr(start, end - start + 2));

    // Passe à la position après \r\n
    start = end + 2;
  }
  return ( hash );
}

static std::vector<t_token> lx_vector_line(const std::string& str)
{
    std::vector<t_token> tokens;
    size_t start = 0;
    bool in_header_value = false;  // Flag pour indiquer qu'on est dans une valeur de header

    while (start < str.size())
    {
        // Cherche les délimiteurs : espace ou \r\n
        size_t space_pos = str.find(' ', start);
        size_t crlf_pos = str.find("\r\n", start);

        // Détermine le prochain délimiteur le plus proche
        size_t next_pos = std::string::npos;
        bool is_crlf = false;

        // Vérifie si \r\n est présent et est le premier délimiteur
        if (crlf_pos != std::string::npos)
        {
            next_pos = crlf_pos;
            is_crlf = true;
        }

        // Compare avec l'espace
        if (space_pos != std::string::npos &&
            (next_pos == std::string::npos || space_pos < next_pos))
        {
            next_pos = space_pos;
            is_crlf = false;
        }

        // Si aucun délimiteur n'est trouvé, prends le reste de la chaîne
        if (next_pos == std::string::npos)
        {
            tokens.push_back(t_token(UNKNOWN, str.substr(start)));
            break;
        }

        // Extrait le token avant le délimiteur
        if (next_pos > start)
        {
            // Si on est dans une valeur de header, on ne découpe pas sur ":"
            if (!in_header_value)
            {
                // Cherche le premier ":" dans la sous-chaîne
                size_t colon_pos = str.find(':', start);
                if (colon_pos != std::string::npos && colon_pos < next_pos)
                {
                    // Ajoute la clé du header
                    tokens.push_back(t_token(UNKNOWN, str.substr(start, colon_pos - start)));
                    // Ajoute le ":"
                    tokens.push_back(t_token(COLON, ":"));
                    // Ajoute la valeur du header (inclut les ":" suivants)
                    tokens.push_back(t_token(UNKNOWN, str.substr(colon_pos + 1, next_pos - colon_pos - 1)));
                    //tokens.push_back(t_token(UNKNOWN, str.substr(colon_pos + 1, next_pos - crlf_pos - 1)));
                    in_header_value = true;  // On est maintenant dans une valeur de header
                }
                else
                {
                    // Ajoute le token (pas de ":" trouvé)
                    tokens.push_back(t_token(UNKNOWN, str.substr(start, next_pos - start)));
                }
            }
            else
            {
                // On est dans une valeur de header, on ajoute tout jusqu'au délimiteur
                //tokens.push_back(t_token(UNKNOWN, str.substr(start, next_pos - start)));
                tokens.push_back(t_token(UNKNOWN, str.substr(start, crlf_pos - start)));
            }
        }

        // Ajoute le délimiteur comme token
        if (is_crlf)
        {
            tokens.push_back(t_token(CRLF, "\r\n"));
            start = next_pos + 2;
            in_header_value = false;  // Réinitialise pour la prochaine ligne
        }
        else if (str[next_pos] == ' ')
        {
            tokens.push_back(t_token(WHITESPACE, " "));
            start = next_pos + 1;
        }
    }

    return tokens;
}

static std::vector<t_token> lx_write_value(const std::vector<std::string>& hash)
{
    std::vector<t_token> lexer;
    size_t index = 0;

    while (index < hash.size())
    {
        // Convertit la ligne actuelle en tokens
        std::vector<t_token> line_tokens = lx_vector_line(hash[index]);
        lexer.insert(lexer.end(), line_tokens.begin(), line_tokens.end());

        // Vérifie si les deux derniers tokens sont des CRLF (fin des headers)
        if (lexer.size() >= 2 &&
            lexer[lexer.size() - 1].type == CRLF &&
            lexer[lexer.size() - 2].type == CRLF)
        {
            break;  // Arrête la boucle : fin des headers détectée
        }

        index++;
    }

    return lexer;
}

static void lx_tokenizer(std::vector<t_token>& lexer)
{
    if (lexer.empty()) return;

    // Parcourt tous les tokens pour les typer
    for (size_t i = 0; i < lexer.size(); ++i)
    {
        t_token& token = lexer[i];

        // --- Typage de la Request Line (ex: "GET / HTTP/1.1\r\n")
        if (i == 0 && (token.value == "GET" || token.value == "POST" || token.value == "DELETE"))
        {
            token.type = METHOD;
        }
        else if (i == 2 && !token.value.empty() && token.value[0] == '/')
        {
            token.type = PATH;
        }
        else if (i == 4 && token.value == "HTTP/1.1")
        {
            token.type = VERSION;
        }
        // --- Typage des headers (ex: "Host: localhost\r\n")
        else if (token.value == ":")
        {
            token.type = COLON;
        }
        else if (token.value == " ")
        {
            token.type = WHITESPACE;
        }
        else if (token.value == "\r\n")
        {
            token.type = CRLF;
        }
        // --- Typage des clés et valeurs de headers
        else if (i > 0)
        {
            // Si le token précédent est un COLON, alors c'est une HEADER_VALUE
            if (i > 0 && lexer[i - 3].type == COLON)
            {
                token.type = HEADER_VALUE;
            }
            // Si le token suivant est un COLON, alors c'est une HEADER_KEY
            else if (i + 1 < lexer.size() && lexer[i + 1].type == COLON)
            {
                token.type = HEADER_KEY;
            }
        }
    }
}

static std::string extract_body(const std::string& raw_request)
{
    // Cherche la position de "\r\n\r\n" (fin des headers)
    size_t body_start = raw_request.find("\r\n\r\n");

    // Si "\r\n\r\n" n'est pas trouvé, il n'y a pas de body
    if (body_start == std::string::npos)
    {
        return ( "" );  // Retourne une chaîne vide
    }

    // Extrait tout ce qui suit "\r\n\r\n"
    return raw_request.substr(body_start + 4);  // +4 pour sauter "\r\n\r\n"
}

std::vector<t_token> HttpHandler::lexing(const std::string raw_request)
{
  std::vector<t_token>      lexer;
  std::vector<std::string>  hash;

  hash = lx_hash_request(raw_request);
  if (hash.empty())
  {
    std::cerr << COLOR_RED << "ERROR:: has request lexer failure" << COLOR_RESET << std::endl;
    return ( lexer ); 
  }

  lexer = lx_write_value(hash);
  if (lexer.empty())
  {
    std::cerr << COLOR_RED << "ERROR:: write value in lexer failure" << COLOR_RESET << std::endl;
    return( lexer );
  }

  lx_tokenizer(lexer);
  if (lexer.empty())
  {
    std::cerr << COLOR_RED << "ERROR:: tokenizer in lexer failure" << COLOR_RESET << std::endl;
    return( lexer );
  }

  lexer.push_back(t_token(BODY, extract_body(raw_request)));
#if DEBUG_FLAG
  print_hash(hash);
  printTokens(lexer);
#endif 
  return ( lexer );
}

/* =========================================================================== */
/*
static void lx_tokenizer(std::vector<t_token>& lexer)
{
  int curr = lexer.size() - 1;
  int i = lexer.at(curr).value.size() - 1;

  if (curr == 0 && (lexer.at(curr).value == "GET" || lexer.at(curr).value == "POST" || lexer.at(curr).value == "DELETE"))
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
  else if (curr >= 5 && !lexer.at(curr).value.empty() && lexer.at(curr).value.at(i) == ':' && lexer.at(curr - 1).type != BODY)
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
  else if (curr >= 8 && ( lexer.at(curr - 1).type == CRLF || lexer.at(curr - 1).type == BODY))
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
  size_t index = 0;
  std::vector<t_token> lexer;

  while (index + 1 < raw_request.size())
  {
    lx_write_value(raw_request, index, lexer);
    lx_tokenizer(lexer);
  }
  
#if DEBUG_FLAG
  std::cout << raw_request << std::endl;
  printTokens(lexer);
#endif  
  return ( lexer );
}
*/
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
  
  while (index < lexer.size() && lexer[index].type != CRLF)
  {
    if (index + 5 >= lexer.size())
    {
      std::cout << COLOR_RED << "Error : here 1" << COLOR_RESET << std::endl;
      return ( false );
    }

    if(lexer[index].type != HEADER_KEY)
    {
      std::cout << COLOR_RED << "Error : here 2" << lexer[index].value << COLOR_RESET << std::endl;
      return ( false );
    }
    if (lexer[index + 1].type != COLON)
    {
      std::cout << COLOR_RED << "Error : here 3" << lexer[index + 1].value <<COLOR_RESET << std::endl;
      return ( false );
    }
    if (lexer[index + 4].type != HEADER_VALUE)
    {
      std::cout << COLOR_RED << "Error : here 4" << lexer[index + 4].value <<COLOR_RESET << std::endl;
      return ( false );
    }
    if(lexer[index + 5].type != CRLF)
    {
      std::cout << COLOR_RED << "Error : here 5" << lexer[index + 5].value <<COLOR_RESET << std::endl;
      return ( false );
    }

    index += 6;
  }
  return ( true );
}

/*
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
*/

static bool dd_end_of_headers(const std::vector<t_token>& lexer)
{
    // Vérifie que les deux derniers tokens sont des CRLF (fin des headers)
    if (lexer.size() < 2)
        return false;
    size_t last = lexer.size() - 2;
    size_t prev = lexer.size() - 3;
    return (lexer[last].type == CRLF && lexer[prev].type == CRLF);
}

bool  HttpHandler::descending_deriv(std::vector<t_token>& lexer)
{
  if (!dd_first_line(lexer))
  {
    std::cout << COLOR_RED << "Error : descending deriv bad first line" << COLOR_RESET << std::endl;
    return ( false );
  }
  if (!dd_header_line(lexer))
  {
    std::cout << COLOR_RED << "Error : descending deriv bad header line" << COLOR_RESET << std::endl;
    return ( false );
  }
  if (!dd_end_of_headers(lexer))  // Vérifie la fin des headers
  { 
    std::cout << COLOR_RED << "Error : descending deriv bad end of header" << COLOR_RESET << std::endl;
    return (false);
  }
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
      h_key = lexer[index].value ;
      h_key.erase(std::remove(h_key.begin(), h_key.end(), ':'), h_key.end());
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
  else if (token.type == BODY && token.value == "")
    request.body += '\n';
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


