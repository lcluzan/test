/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerConfig.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tjacquel <tjacquel@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/20 19:26:05 by lcluzan           #+#    #+#             */
/*   Updated: 2026/04/24 20:50:21 by tjacquel         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVERCONFIG_HPP
# define SERVERCONFIG_HPP

# include <map>
# include <fstream>
# include <iostream>
# include <sstream>
# include <string>
# include <vector>
# include <cstdlib>

typedef struct	s_block {
	// std::vector<t_ServerParser>	context;
	std::string					name;
	std::vector< std::vector<std::string> >	directives;
	std::vector<std::string>	parameters;
	std::vector<s_block>		embed_block;
}				t_block;

class ServerConfig {

public:
	ServerConfig();
	~ServerConfig();

    int port;
    std::string server_name;
} ;

t_block		parseTokens(const std::vector<std::string>& tokens, size_t& i);
void		printParserBlocks(const t_block& curr_block, const std::string& prefix, int indent_level);
void		printParserEmbedBlocks(const std::vector<t_block>& embed_block, size_t& i);

#endif
