/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerConfig.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tjacquel <tjacquel@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/20 19:42:36 by lcluzan           #+#    #+#             */
/*   Updated: 2026/06/11 17:18:17 by tjacquel         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <config/ServerConfig.hpp>

ServerConfig::ServerConfig() : host("0.0.0.0"), port(8000), client_max_body_size(8000000) { }

ServerConfig::~ServerConfig() {}

std::string								ServerConfig::getHost() const {
	return (this->host);
}

int										ServerConfig::getPort() const {
	return (this->port);
}

int										ServerConfig::getClientMaxBodySize() const {
	return (this->client_max_body_size);
}

std::map<std::string, LocationConfig>	ServerConfig::getLocationConfig() const {
	return (this->location);
}

void	ServerConfig::printServerConfig(const size_t i) const {
	std::cout << "\nvirtual_servers[" << i << "].host=" << this->host << std::endl;
	std::cout << "virtual_servers[" << i << "].port=" << this->port << std::endl;
	std::cout << "virtual_servers[" << i << "].clients_max_body_size=" << this->client_max_body_size << std::endl;

	if (this->location.size() > 0) {
		std::map<std::string, LocationConfig>::const_iterator	it;
		for (it = location.begin();  it != location.end(); ++it) {
			std::cout << "virtual_servers[" << i << "].location.KEY=" << it->first << std::endl;
			it->second.printLocationConfig();
		}
	}

// Usage in main
/*
	for (size_t i = 0; i < virtual_servers.size(); i++) {
		virtual_servers[i].printServerConfig(i);
	}
*/

}

void	checkEmptyDirBlock(const t_block& curr_block) {
	std::ostringstream	thw;
	if (curr_block.directives.empty()) {
		thw << "Config file error: cannot have empty directive block";
		throw std::logic_error(thw.str());
	}
}

void	ServerConfig::serverDirCheck(const t_block& curr_block) {
	std::ostringstream	thw;

	if (!curr_block.parameters.empty()) {
		thw << "Config file error: no parameter expected for server directive";
		throw std::logic_error(thw.str());
	}
	if (curr_block.directives.empty() && !curr_block.embed_block.empty()) {
		thw << "Config file error: cannot have empty directive block";
		throw std::logic_error(thw.str());
	}
}

void	ServerConfig::handleAddress(const std::string& addrStr) {
	std::istringstream	iss(addrStr);
	std::vector<int>	addrVec;
	std::string			addrToken;
	std::ostringstream	thw;

	while (std::getline(iss, addrToken, '.')) {
		if (addrToken.empty()) {
			thw << "Config file error: IPv4 address cannot be empty in " << addrStr;
			throw std::logic_error(thw.str());
		}
		addrVec.push_back(ft_stoi(addrToken));
	}

	if (addrVec.size() != 4) {
		thw << "Config file error: IPv4 address format error";
		throw std::logic_error(thw.str());
	}

	for (size_t i = 0; i < addrVec.size(); ++i) {
		if (addrVec.at(i) < 0 || addrVec.at(i) > 255) {
			thw << "Config file error: IPv4 address range must be between 0 and 255, found " << addrVec.at(i);
			throw std::logic_error(thw.str());
		}
	}

	this->host = addrStr;

}

void	ServerConfig::handlePort(const std::string& portStr) {
	std::ostringstream	thw;

	this->port = ft_stoi(portStr);

	if (this->port < 1 || this->port > 65535) {
		thw << "Config file error: port value must be within 1 and 65535, found " << this->port;
		throw std::logic_error(thw.str());
	}
}

void	ServerConfig::listenDirCheckAndLoad(const std::vector<std::string>& curr_dir) {
	std::ostringstream	thw;
	if (curr_dir.size() > 2) {
		thw << "Config file error: at least one argument expected for listen directive, found " << curr_dir.size() - 1 << " arguments.";
		throw std::logic_error(thw.str());
	}

	size_t	foundDQuote = curr_dir.at(1).find(":");
	if (foundDQuote != std::string::npos) {
		std::string	addrStr = curr_dir.at(1).substr(0, foundDQuote);
		std::string	portStr = curr_dir.at(1).substr(foundDQuote+1);
		handleAddress(addrStr);
		handlePort(portStr);
	}
	else {
		size_t	foundDot = curr_dir.at(1).find(".");
		if (foundDot != std::string::npos) {
			handleAddress(curr_dir.at(1));
		}
		else {
			handlePort(curr_dir.at(1));
		}
	}
}

void	ServerConfig::cmbsDirCheckAndLoad(const std::vector<std::string>& curr_dir) {
	// I chose to rewrite client_max_body_size if there was a new directive
	std::ostringstream	thw;
	if (curr_dir.size() != 2) {
		thw << "Config file error: single mandatory argument expected for client_max_body_size directive, found " << curr_dir.size() - 1 << " argument(s).";
		throw std::logic_error(thw.str());
	}

	std::string		arg = curr_dir.at(1);
	char			last_char = arg.at(arg.length() - 1);
	unsigned long	multiplier = 1;

	// kilobit
	if (last_char == 'k' || last_char == 'K') {
		multiplier = 1024;
		arg = arg.substr(0, arg.length() - 1);
	}
	// megabit
	else if (last_char == 'm' || last_char == 'M' ) {
		multiplier = 1048576;
		arg = arg.substr(0, arg.length() - 1);
	}
	// gigabit
	else if (last_char == 'g' || last_char == 'G') {
		multiplier = 1073741824;
		arg = arg.substr(0, arg.length() - 1);
	}

	int				base_value = ft_stoi(arg)	;
	unsigned long	final_value = static_cast<unsigned long>(base_value) * multiplier;

	if (final_value > CLIENT_MAX_BODY_SIZE_LIMIT) {
		thw << "Config file error: client_max_body_size exceeds or is equal to 2GB maximum limit";
		throw std::logic_error(thw.str());
	}

	this->client_max_body_size = static_cast<int>(final_value);
}

void	ServerConfig::loadFromBlock(const t_block& curr_block) {
	std::ostringstream	thw;
	if (curr_block.name != "server") {
		thw << "Config file error: expected server directive block, found " << curr_block.name;
		throw std::logic_error(thw.str());
	}
	serverDirCheck(curr_block);
	for (size_t i = 0; i < curr_block.directives.size(); i++) {
		if (curr_block.directives[i][0] == "listen") {
			listenDirCheckAndLoad(curr_block.directives[i]);
		}
		else if (curr_block.directives[i][0] == "client_max_body_size") {
			cmbsDirCheckAndLoad(curr_block.directives[i]);
		}
		else {
			thw << "Config file error: directive `" << curr_block.directives[i][0] << "` found in server block is not supported";
			throw std::logic_error(thw.str());
		}

	}

	for (size_t i = 0; i < curr_block.embed_block.size(); i++) {

		if (curr_block.embed_block[i].name == "location") {
			LocationConfig	new_loc;
			new_loc.loadFromEmbedBlock(curr_block.embed_block[i]);

			this->location[curr_block.embed_block[i].parameters[0]] = new_loc;
		}

		else {
			thw << "Config file error: embedded block `" << curr_block.embed_block[i].name << "` is not supported";
			throw std::logic_error(thw.str());
		}
	}

}