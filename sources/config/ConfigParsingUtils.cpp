/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigParsingUtils.cpp                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tjacquel <tjacquel@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/26 19:54:50 by tjacquel          #+#    #+#             */
/*   Updated: 2026/06/08 14:23:15 by tjacquel         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <config/ConfigParsingUtils.hpp>

bool	onlyDigits(const std::string string) {
	for (size_t i = 0; i < string.length(); ++i) {
		if (string.at(i) > '9' || string.at(i) < '0') {
			return (false);
		}
	}
	return (true);
}

int		ft_stoi(const std::string str) {
	std::ostringstream	thw;

	if (str.empty()) {
		thw << "[ft_stoi]: empty string detected";
		throw std::logic_error(thw.str());
	}

	if (!onlyDigits(str)) {
		thw << "[ft_stoi]: non-digit found in `" << str << "`";
		throw std::logic_error(thw.str());
	}

	long long	res = 0;
	for (size_t i = 0; i < str.length() && str.at(i) >= '0' && str.at(i) <= '9'; ++i) {
		res = res * 10 + (str.at(i) - '0');
		if (res > 2147483647 || res < -2147483648) {
			thw << "[ft_stoi]: overflow detected in `" << str << "`";
			throw std::overflow_error(thw.str());
		}
	}

	return(static_cast<int>(res));
}