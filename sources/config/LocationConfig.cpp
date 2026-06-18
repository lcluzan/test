/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   LocationConfig.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lcluzan <lcluzan@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/23 17:23:36 by tjacquel          #+#    #+#             */
/*   Updated: 2026/06/17 20:05:48 by lcluzan          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <config/LocationConfig.hpp>

LocationConfig::LocationConfig() : root(""), autoindex(false), upload_enable(false), redirect(0, "") { }

LocationConfig::~LocationConfig() { }

std::string		LocationConfig::getRoot() const {
	return (this->root);
}

std::set<std::string>	LocationConfig::getMethods() const {
	return (this->methods);
}

std::string				LocationConfig::getIndex() const {
	return (this->index);
}

bool					LocationConfig::getAutoindex() const {
	return (this->autoindex);
}

bool					LocationConfig::getUploadEnable() const {
	return (this->upload_enable);
}

std::string				LocationConfig::getUploadLoc() const {
	return (this->upload_loc);
}

std::pair<int, std::string>	LocationConfig::getRedirect() const {
	return (this->redirect);
}

std::map<int, std::string>	LocationConfig::getErrorPage() const {
	return (this->error_page);
}

std::map<std::string, std::string>	LocationConfig::getCgiPass() const {
	return (this->cgi_pass);
}

bool					LocationConfig::checkMethod(const std::string& str) const {
	if (this->methods.find(str) != this->methods.end()) {
		return (true);
	}
	return (false);
}

void	LocationConfig::printLocationConfig() const {
	std::cout << "    root=" << this->root << std::endl;
	if (this->methods.size() > 0) {
		std::cout << "    methods=";
		size_t	i = 0;
		for (std::set<std::string>::const_iterator it = this->methods.begin(); it != this->methods.end(); ++it) {
			std::cout << *it;
			if (i != this->methods.size() - 1) {
				std::cout << ", ";
			}
			i++;
		}
		std::cout << std::endl;
	}
	if (this->index.size() > 0)
		std::cout << "    index=" << this->index << std::endl;
	if (this->autoindex)
		std::cout << "    autoindex=on" << std::endl;
	if (this->upload_enable)
		std::cout << "    upload_enable=on" << std::endl;
	if (this->upload_loc.size() > 0) {
		std::cout << "    upload_loc=" << this->upload_loc << std::endl;
	}

	if (this->redirect.first != 0) {
		std::cout << "    redirect " << redirect.first << "=" << redirect.second << std::endl;
	}

	if (this->error_page.size() > 0) {
		std::map<int, std::string>::const_iterator it;
		for (it = this->error_page.begin(); it != this->error_page.end(); ++it) {
			std::cout << "    error_page " << it->first << "=" << it->second << std::endl;
		}
	}
	if (!this->cgi_pass.empty()) {
		std::map<std::string, std::string>::const_iterator it;
		for (it = this->cgi_pass.begin(); it != this->cgi_pass.end(); ++it) {
			std::cout << "    cgi_pass[" << it->first << "]=" << it->second << std::endl;
		}
	}
}

bool	LocationConfig::isValidCgiPair(const std::string& extension, const std::string executable) const {
	if (extension == ".py" && executable.find("python3") != std::string::npos) {
		return true;
	}
	if (extension == ".php" && executable.find("php-cgi") != std::string::npos) {
		return true;
	}
	if (extension == ".sh" && executable.find("bash") != std::string::npos) {
		return true;
	}
	return (false);
}

void	LocationConfig::parseCgiPass(const std::vector<std::string>& cgiPassDir) {
	std::ostringstream	thw;
	if (cgiPassDir.size() != 3) {
		thw << "Config file error: double mandatory argument exepected for cgi_pass directive, found " << cgiPassDir.size() - 1;
		throw std::logic_error(thw.str());
	}

	std::string extension = cgiPassDir.at(1);
	std::string executable = cgiPassDir.at(2);

	// 1. Duplicate check
	if (this->cgi_pass.find(extension) != this->cgi_pass.end()) {
		thw << "Config file error: duplicate cgi_pass directive found for extension `" << extension << "`";
		throw std::logic_error(thw.str());
	}

	// 2. Valid pair check
	if (!isValidCgiPair(extension, executable)) {
		thw << "Config file error: `" << extension << "-" << executable << "` extension-executable pair found is not supported for cgi_pass directive";
		throw std::logic_error(thw.str());
	}

	// 3. Existence and permission check
	if (access(executable.c_str(), X_OK) != 0) {
		thw << "Config file error: CGI executable not found or missing execution rights at `" << executable << "`";
		throw std::runtime_error(thw.str());
	}

	this->cgi_pass.insert(std::pair<std::string, std::string>(extension, executable));
}

void	LocationConfig::errorpageDirCheckAndLoad(const std::vector<std::string>& error_pageDir) {
	std::ostringstream	thw;
	if (error_pageDir.size() < 3) {
		thw << "Config file error: at least two arguments expected for error_page directive, found " << error_pageDir.size() - 1;
		throw std::logic_error(thw.str());
	}

	std::vector<int>	err_code;
	size_t	i = 1;
	for (; i < error_pageDir.size(); i++) {
		if (onlyDigits(error_pageDir[i])) {
			err_code.push_back(ft_stoi(error_pageDir[i]));
		}
		else {
			break ;
		}
	}

	if (i != error_pageDir.size() - 1) {
		thw << "Config file error: URI location expected as unique last error_page directive argument, found " << error_pageDir.size() - i << " arguments left";
		throw std::logic_error(thw.str());
	}

	for (size_t j = 0; j < err_code.size(); ++j) {
		// error_code min-max check
		if (err_code.at(j) < 300 || err_code.at(j) > 599) {
			thw << "Config file error: value `" << err_code.at(j) << "` must be between 300 and 599 in error_page directive";
			throw std::logic_error(thw.str());
		}
	}

	std::string	uri = error_pageDir.at(error_pageDir.size() - 1);
	i = 0;
	for (; i < err_code.size(); i++) {
		int	code = err_code.at(i);
		this->error_page.insert(std::pair<int, std::string>(code, uri));
		// error_code duplicate check (we keep the first instance like nginx)
		// we go through the already existing map container to check if the error_code value is already stored (std::map::insert() does it under the hood)
		// if no duplicate found we store the error_code associated with its URI
		// else we simply ignore/do not store the error_code=URI pair

	}
}

void	LocationConfig::redirectDirCheckAndLoad(const std::vector<std::string>& returnDir) {
	std::ostringstream	thw;

	// Arguments count
	if (returnDir.size() != 3) {
		thw << "Config file error: redirect directive expects exactly 2 arguments, found " << returnDir.size() - 1;
		throw std::logic_error(thw.str());
	}

	// Duplicate check
	if (this->redirect.first != 0) {
		thw << "Config file error: redirect directive duplicate detected";
		throw std::logic_error(thw.str());
	}

	int	code;
	code = ft_stoi(returnDir.at(1));

	// Status code value check
	if (code < 300 || code > 399) {
		thw << "Config file error: redirect status code value must be within 300 and 399, found " << code;
		throw std::logic_error(thw.str());
	}

	std::string	uri = returnDir.at(2);
	this->redirect.first = code;
	this->redirect.second = uri;
}

void	LocationConfig::uploadlocDirCheckAndLoad(const std::vector<std::string>& upload_locDir) {
	std::ostringstream	thw;

	if (upload_locDir.size() != 2) {
		thw << "Config file error: single mandatory argument expected for upload_loc directive, found " << upload_locDir.size() - 1;
		throw std::logic_error(thw.str());
	}
	this->upload_loc = upload_locDir.at(1);

}

void	LocationConfig::uploadenbableDirCheckAndLoad(const std::vector<std::string>& upload_enableDir) {
	std::ostringstream	thw;

	if (upload_enableDir.size() != 2) {
		thw << "Config file error: single mandatory argument expected for upload_enable directive, found " << upload_enableDir.size() - 1;
		throw std::logic_error(thw.str());
	}

	if		(upload_enableDir.at(1) == "on")	{ this->upload_enable = true; }
	else if	(upload_enableDir.at(1) == "off")	{ this->upload_enable = false; }
	else {
		thw << "Config file error: argument `" << upload_enableDir.at(1) << "` found in upload_enable directive is unsupported";
		throw std::logic_error(thw.str());
	}
}

void	LocationConfig::autoindexDirCheckAndLoad(const std::vector<std::string>& autoindexDir) {
	std::ostringstream	thw;

	if (autoindexDir.size() != 2) {
		thw << "Config file error: single mandatory argument expected for autoindex directive, found " << autoindexDir.size() - 1;
		throw std::logic_error(thw.str());
	}

	if		(autoindexDir.at(1) == "on")	{ this->autoindex = true; }
	else if	(autoindexDir.at(1) == "off")	{ this->autoindex = false; }
	else {
		thw << "Config file error: argument `" << autoindexDir.at(1) << "` found in autoindex directive is unsupported";
		throw std::logic_error(thw.str());
	}

}

void	LocationConfig::indexDirCheckAndLoad(const std::vector<std::string>& indexDir) {
	std::ostringstream	thw;

	if (indexDir.size() != 2) {
		thw << "Config file error: single mandatory argument expected for index directive, found " << indexDir.size() - 1;
		throw std::logic_error(thw.str());
	}
	this->index = indexDir.at(1);
}

void	LocationConfig::methodsDirCheckAndLoad(const std::vector<std::string>& curr_dir) {
	std::ostringstream	thw;

	if (curr_dir.size() < 2) {
		thw << "Config file error: at least one argument expected for methods directive, found " << curr_dir.size() - 1;
		throw std::logic_error(thw.str());
	}

	for (size_t i = 1; i < curr_dir.size(); ++i) {
		// std::cout << "allow_methodsDir.at(" << i << ")=" << curr_dir.at(i) << std::endl;
		if (curr_dir.at(i) == "GET") {
			methods.insert("GET");
		}
		else if (curr_dir.at(i) == "POST") {
			methods.insert("POST");
		}
		else if (curr_dir.at(i) == "DELETE") {
			methods.insert("DELETE");
		}
		// else if (curr_dir.at(i) == "UPLOAD") {
		// 	methods.push_back("UPLOAD");
		// }
		else {
			thw << "Config file error: `" << curr_dir.at(i) << "` found in methods directive is unsupported";
			throw std::logic_error(thw.str());
		}
		// methods.insert(curr_dir.at(i));
	}
}

void	LocationConfig::rootDirCheckAndLoad(const std::vector<std::string>& rootDirective) {
	std::ostringstream	thw;

	if (rootDirective.size() != 2) {
		thw << "Config file error: single mandatory argument expected for root directive, found " << rootDirective.size() - 1;
		throw std::logic_error(thw.str());
	}
	if (root != "") {
		thw << "Config file error: duplicate root directive found";
		throw std::logic_error(thw.str());
	}
	this->root = rootDirective.at(1);
}

void	LocationConfig::loadFromEmbedBlock(const t_block& embed_block) {
	std::ostringstream	thw;

	// std::cout << "embed_block.name=" << embed_block.name << std::endl;
	// std::cout << "embed_block.parameters.size()=" << embed_block.parameters.size() << std::endl;
	// for (size_t i = 0; i < embed_block.parameters.size(); ++i) {
	// 	std::cout << "embed_block.parameters[" << i << "]=" << embed_block.parameters[i] << std::endl;
	// }
	if (embed_block.parameters.size() != 1) {
		thw << "Config file error: single mandatory parameter expected for location context directive, found " << embed_block.parameters.size();
		throw std::logic_error(thw.str());
	}

	if (!embed_block.embed_block.empty()) {
		thw << "Config file error: nested blocks within embedded location directive are not supported";
		throw std::logic_error(thw.str());
	}

	for (size_t i = 0; i < embed_block.directives.size(); i++) {
		if (embed_block.directives[i][0] == "root")
			{ rootDirCheckAndLoad(embed_block.directives[i]); }
		else if (embed_block.directives[i][0] == "methods") methodsDirCheckAndLoad(embed_block.directives[i]);
		else if (embed_block.directives[i][0] == "index") {
			indexDirCheckAndLoad(embed_block.directives[i]);
		}
		else if (embed_block.directives[i][0] == "autoindex") { autoindexDirCheckAndLoad(embed_block.directives[i]); }
		else if (embed_block.directives[i][0] == "upload_enable") { uploadenbableDirCheckAndLoad(embed_block.directives[i]); }
		else if (embed_block.directives[i][0] == "upload_loc") { uploadlocDirCheckAndLoad(embed_block.directives[i]); }
		else if (embed_block.directives[i][0] == "redirect") { redirectDirCheckAndLoad(embed_block.directives[i]); }
		else if (embed_block.directives[i][0] == "error_page") { errorpageDirCheckAndLoad(embed_block.directives[i]); }
		else if (embed_block.directives[i][0] == "cgi_pass") { parseCgiPass(embed_block.directives[i]); }

		else {
			thw << "Config file error: directive `" << embed_block.directives[i][0] << "` found in location context directive is unsupported";
			throw std::logic_error(thw.str());
		}
	}
}
