/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigParsing.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tjacquel <tjacquel@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/23 17:51:28 by tjacquel          #+#    #+#             */
/*   Updated: 2026/06/20 21:25:28 by tjacquel         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIGPARSING_HPP
# define CONFIGPARSING_HPP

# include <string>
# include <vector>

# define PRINT_SERVER_CONFIG 1
# define PRINT_TOKEN_CONFIG  0


class ServerConfig;

typedef struct	s_block {
	std::string								name;
	std::vector<std::string>				parameters;
	std::vector< std::vector<std::string> >	directives;
	std::vector<s_block>					embed_block;
}				t_block ;

void	confParsingHandler(const char* filepath, std::vector<ServerConfig>& server);


#endif
