/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CommandParser.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: soksak <soksak@42istanbul.com.tr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/07 19:26:48 by soksak            #+#    #+#             */
/*   Updated: 2025/09/07 21:13:15 by soksak           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef COMMANDPARSER_HPP
#define COMMANDPARSER_HPP

#include <string>
#include <vector>
#include <sstream>
#include <algorithm>
#include "IRCMessage.hpp"

class CommandParser
{
	public:
		static IRCMessage parseMessage(const std::string& message);
		static std::vector<std::string> splitString(const std::string& str, char delimiter);
		static std::string trim(const std::string& str);
};

#endif
