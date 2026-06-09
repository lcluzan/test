/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigParsing.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tjacquel <tjacquel@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/23 18:37:03 by tjacquel          #+#    #+#             */
/*   Updated: 2026/06/04 17:14:12 by tjacquel         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <config/ServerConfig.hpp>
#include <config/ConfigParsing.hpp>

static void		printParserBlocks(const t_block& curr_block, const std::string& prefix, int indent_level) {
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

// Usage in main
/*
	for (size_t i = 0; i < block.size(); i++) {
		std::stringstream	ss;
		ss << "block[" << i << "]";
		std::string			base_prefix = ss.str();

		printParserBlocks(block[i], base_prefix, 0);
		std::cout << "\n";
	}
*/

}

static void		printTokens(std::vector<std::string>& tokens) {
	for (size_t i = 0; i < tokens.size(); i++) {
		std::cout << "token[" << i << "]=" << "`" << tokens[i] << "`" << std::endl;
	}
}

static void		printLines(std::vector<std::string>& lines) {
	for (size_t i = 0; i < lines.size(); i++) {

	}
}

static void		linterBlock(const t_block& curr_block, size_t& i) {
	std::ostringstream	thw;
	if (curr_block.directives.empty() && curr_block.embed_block.empty()) {
		thw << "Config file error: cannot have empty directive block\n";
		thw << "    Found no directive or embed block within: block[" << i << "]=" << curr_block.name;
		throw std::logic_error(thw.str());
	}
	if (!curr_block.embed_block.empty()) {
		for (size_t j = 0; j < curr_block.embed_block.size(); j++) {
			linterBlock(curr_block.embed_block[j], j);
		}
	}
}

static void		paramCheck(const t_block& curr_block) {
	std::ostringstream	thw;
	if (curr_block.name == "server") {
		if (!curr_block.parameters.empty()) {
			thw << "Config file error: no parameter expected for server directive";
			throw std::logic_error(thw.str());
		}
	}
	if (curr_block.name == "location") {
		if (curr_block.parameters.empty()) {
			thw << "Config file error: filepath parameter expected for location directive";
			throw std::logic_error(thw.str());
		}
		else if (curr_block.parameters.size() > 1) {
			thw << "Config file error: single parameter exepected for location directive";
			throw std::logic_error(thw.str());
		}
	}
	if (!curr_block.embed_block.empty()) {
		for (size_t j = 0; j < curr_block.embed_block.size(); j++) {
			paramCheck(curr_block.embed_block[j]);
		}
	}
}

static t_block		parseTokens(const std::vector<std::string>& tokens, size_t& i) {
	t_block				block;
	std::ostringstream	thw;

	block.name = tokens[i];
	i++;

	while (i < tokens.size() && tokens[i] != "{") {
		if (tokens.at(i) == "}" || tokens.at(i) == ";") {
			thw << "Config file error: unexpected token `" << tokens.at(i) << "`";
			throw std::logic_error(thw.str());
		}
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
				thw << "Config file error: missing `;` before key token or EOF at tokens[" << i << "]=" << tokens[i];
				throw std::logic_error(thw.str());
			}
		}
	}

	if (i < tokens.size() && tokens[i] == "}") {
		i++;
	}

	return (block);
}

static void	astBuilder(std::vector<t_block>& block, std::vector<std::string>& tokens) {
	std::ostringstream	thw;
	size_t i = 0;
	while (i < tokens.size()) {
		if (tokens[i] == "server") {
			block.push_back(parseTokens(tokens, i));
		}
		else {
			thw << "Config file error: Unexpected token found outside server block: tokens[" << i << "]=" << tokens[i];
			throw std::logic_error(thw.str());
		}
	}

	if (block.empty()) {
		thw << "Config file error: no valid block constructed";
		throw std::logic_error(thw.str());
	}
}

static void	syntaxLinter(std::vector<std::string> tokens) {
	int	bracket_count = 0;
	std::ostringstream	thw;
	for (size_t i = 0; i < tokens.size(); i++) { // bracket counting
		if (tokens[i] == "{") {
			bracket_count++;
		}
		if (tokens[i] == "}") {
			bracket_count--;
		}
		if (bracket_count < 0) {
			thw << "too many closing brackets in config file";
			throw std::logic_error(thw.str());
		}
	}
	if (bracket_count != 0) {
		thw << "Config file error: closing bracket count doest not match open bracket count";
		throw std::logic_error(thw.str());
	}

	bracket_count = 0;
	bool	in_brackets = false;
	for (size_t i = 0; i < tokens.size(); i++) {
		if (tokens[i] == "{") {
			bracket_count++;
			in_brackets = true;
		}
		if (tokens[i] == "}") {
			bracket_count--;
			if (bracket_count == 0) {
				in_brackets = false;
			}
		}
		if (in_brackets == false && tokens[i] != "server" && tokens[i] != "}") {
			thw << "Config file error: only `server` token expected outside brackets block but found tokens[" << i << "]=" << tokens[i];
			throw std::logic_error(thw.str());
		}

		if (i > 0 && tokens[i] == ";" && tokens[i - 1] == ";") {
			thw << "Config file error: consecutive separating operator `;` found";
			throw std::logic_error(thw.str());
		}

		if (i > 0 && tokens[i] == "{" && tokens[i - 1] == "{") {
			thw << "Config file error: consecutive open bracket found";
			throw std::logic_error(thw.str());
		}

		if (i > 0 && tokens[i] == "{" && tokens[i - 1] == ";") {
			thw << "Config file error: unexpected `;` found before opening brackets `{`";
			throw std::logic_error(thw.str());
		}
	}

	if (tokens.back() != "}") {
		thw << "Config file error: last token is not a closing bracket";
		throw std::logic_error(thw.str());
	}

}

static void	loadTokens(std::vector<std::string>& tokens, std::vector<std::string>& lines, const char* filepath) {
	for (size_t i = 0; i < lines.size(); i++) {
		size_t j = 0;
		while (j < lines[i].size()) {
			while (j < lines[i].size() && isspace(lines[i][j])) { // skip first whitespace
				j++;
			}
			if (j >= lines[i].size()) {
				break ;
			}
			if (lines[i][j] == '{' || lines[i][j] == '}' || lines[i][j] == ';') { // delimiter token identified
				tokens.push_back(lines[i].substr(j, 1));
				j++;
				continue ;
			}
			size_t	nbegin = j;
			while (j < lines[i].size() && !isspace(lines[i][j]) && lines[i][j] != '{' && lines[i][j] != '}' && lines[i][j] != ';') {
				// word token identified, iterate through j till delimiter
				j++;
			}
			tokens.push_back(lines[i].substr(nbegin, j - nbegin)); // extrack token
		}
	}

	if (PRINT) printLines(lines);

	if (tokens.empty()) {
		std::ostringstream	thw;
		thw << "cannot open input file `" << filepath << "`";
		throw std::runtime_error(thw.str());
	}
}

static void	loadConfigLines(std::vector<std::string>& lines, std::ifstream& inFile) {
	std::string line;
	while (std::getline(inFile, line)) {
		// strip all comments from lines
		size_t  hashPos = line.find("#");
		if (hashPos != std::string::npos) {
		line = line.substr(0, hashPos);
		}
		lines.push_back(line);
	}
}

void	confParsingHandler(const char* filepath, std::vector<ServerConfig>& virtual_servers) {
	std::ifstream inFile(filepath);
	if (!inFile.is_open()) {
		std::ostringstream	thw;
		thw << "cannot open input file `" << filepath << "`";
		throw std::runtime_error(thw.str());
	}

	std::vector<std::string>lines;
	loadConfigLines(lines, inFile);
	inFile.close();


	std::vector<std::string> tokens;
	loadTokens(tokens, lines, filepath);
	if(PRINT) printTokens(tokens);
	syntaxLinter(tokens);


	std::vector<t_block>	block;
	astBuilder(block, tokens);

	for (size_t i = 0; i < block.size(); i++) {
		ServerConfig		new_server;
		new_server.loadFromBlock(block.at(i));
		virtual_servers.push_back(new_server);	
	}

if (PRINT) {
	for (size_t i = 0; i < virtual_servers.size(); i++) {
		virtual_servers[i].printServerConfig(i);
	}
}
}
