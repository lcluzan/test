/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   LocationConfig.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tjacquel <tjacquel@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/23 17:24:59 by tjacquel          #+#    #+#             */
/*   Updated: 2026/06/20 21:20:12 by tjacquel         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LOCATIONCONFIG_HPP
# define LOCATIONCONFIG_HPP

# include <string>
# include <vector>
# include <iostream>
# include <map>
# include <set>
# include <algorithm>
# include <utility>
# include <cstdlib>
# include <unistd.h> // for access()
# include "ConfigParsing.hpp"
# include "ConfigParsingUtils.hpp"

class	LocationConfig {

public:
	LocationConfig();
	~LocationConfig();

	// Parsing methods
	void						loadFromEmbedBlock(const t_block& embed_block);
	void						rootDirCheckAndLoad(const std::vector<std::string>& rootDirective);
	void						methodsDirCheckAndLoad(const std::vector<std::string>& curr_dir);
	void						indexDirCheckAndLoad(const std::vector<std::string>& indexDir);
	void						autoindexDirCheckAndLoad(const std::vector<std::string>& autoindexDir);
	void						redirectDirCheckAndLoad(const std::vector<std::string>& returnDir);
	void						errorpageDirCheckAndLoad(const std::vector<std::string>& error_pageDir);
	void						parseCgiPass(const std::vector<std::string>& cgiPassDir);


	// Getter methods
	std::string							getRoot() const;
	std::set<std::string>				getMethods() const;
	std::string							getIndex() const;
	bool								getAutoindex() const;
	std::pair<int, std::string>			getRedirect() const;
	std::map<int, std::string>			getErrorPage() const;
	std::map<std::string, std::string>	getCgiPass() const;

	bool						checkMethod(const std::string& str) const;

	// Printing method
	void						printLocationConfig() const;

private:
	std::string							root;
	std::set<std::string>				methods; // USAGE: if (this->methods.find("POST") != this->methods.end()) { // POST is allowed! [...] }
	std::string							index;
	bool								autoindex;
	std::pair<int, std::string>			redirect;
	std::map<int, std::string>			error_page;
	std::map<std::string, std::string>	cgi_pass; // KEY=extension, VALUE=executable

} ;

#endif
