/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigParsingUtils.hpp                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tjacquel <tjacquel@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/26 19:54:03 by tjacquel          #+#    #+#             */
/*   Updated: 2026/05/27 13:22:21 by tjacquel         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIGPARSINGUTILS
# define CONFIGPARSINGUTILS

# include <string>
# include <stdexcept>
# include <sstream>

bool	onlyDigits(const std::string string);
int		ft_stoi(const std::string str);


#endif