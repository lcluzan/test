/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigParsing.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tjacquel <tjacquel@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/23 18:37:03 by tjacquel          #+#    #+#             */
/*   Updated: 2026/04/24 21:07:32 by tjacquel         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/ServerConfig.hpp"

void		printParserBlocks(const t_block& curr_block, const std::string& prefix, int indent_level) {
	std::string	indent(indent_level * 4, ' ');

	std::cout << indent << prefix << ".name=" << curr_block.name << "\n";

	if (curr_block.parameters.empty()) {
		std::cout << indent << prefix << ".parameters=NULL\n";
	}
	else {
		for (size_t p = 0; p < curr_block.parameters.size(); p++) {
			std::cout << indent << prefix << ".parameters[" << p << "]=" << curr_block.parameters[p] << "\n";
		}
	}

	indent_level++;
	std::string	dir_indent = indent + "    ";
	for (size_t d = 0; d < curr_block.directives.size(); d++) {
		if (curr_block.directives[d].empty()) { continue ; }
		for (size_t da = 0; da < curr_block.directives[d].size(); da++) {
			std::cout << dir_indent << prefix << ".directives[" << d << "][" << da << "]=" << curr_block.directives[d][da] << "\n";
		}
	}

	for (size_t e = 0; e < curr_block.embed_block.size(); e++) {
		std::stringstream	ss;
		ss << prefix << ".embed_block[" << e << "]";
		std::string			embed_prefix = ss.str();

		printParserBlocks(curr_block.embed_block[e], embed_prefix, indent_level);
	}

}

t_block		parseTokens(const std::vector<std::string>& tokens, size_t& i) {
	t_block	block;

	block.name = tokens[i];
	i++;

	while (i < tokens.size() && tokens[i] != "{") {
		block.parameters.push_back(tokens[i]);
		i++;
	}
	if (i < tokens.size() && tokens[i] == "{") {
		i++;
	}

	while (i < tokens.size() && tokens[i] != "}") {
		if (tokens[i] == "server" || tokens[i] == "location") {
			block.embed_block.push_back(parseTokens(tokens, i));
		}
		else if (tokens[i] == ";") {
			i++;
		}
		else {
			std::vector<std::string> directive;
			while (i < tokens.size() && tokens[i] != ";" && tokens[i] != "{" && tokens[i] != "}") {
				directive.push_back(tokens[i]);
				i++;
			}
			block.directives.push_back(directive);
			if (i < tokens.size() && tokens[i] == ";") {
				i++;
			}
			else {
				std::cerr << "Config file error: missing `;` before key token or EOF at tokens[" << i << "]=" << tokens[i] << std::endl;
				exit (1);
			}
		}
	}

	if (i < tokens.size() && tokens[i] == "}") {
		i++;
	}

	return (block);
}
