/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigParsing.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tjacquel <tjacquel@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/23 17:51:28 by tjacquel          #+#    #+#             */
/*   Updated: 2026/06/08 15:29:26 by tjacquel         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIGPARSING_HPP
# define CONFIGPARSING_HPP

# include <string>
# include <vector>

# define PRINT				0
# define RUN_WITH_SERVER	1


class ServerConfig;

typedef struct	s_block {
	std::string								name;
	std::vector<std::string>				parameters;
	std::vector< std::vector<std::string> >	directives;
	std::vector<s_block>					embed_block;
}				t_block ;

void	confParsingHandler(const char* filepath, std::vector<ServerConfig>& virtual_servers);


#endif
