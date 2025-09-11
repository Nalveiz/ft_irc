/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CommandParser.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: soksak <soksak@42istanbul.com.tr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/07 19:26:48 by soksak            #+#    #+#             */
/*   Updated: 2025/09/11 21:50:23 by soksak           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef COMMANDPARSER_HPP
#define COMMANDPARSER_HPP

#include <string>
#include <vector>
#include <sstream>
#include <algorithm>
#include <iostream>
#include "IRCMessage.hpp"

class CommandParser
{
	public:
		// Main parsing function
		static IRCMessage parseMessage(const std::string& message);

		// Utility functions
		static std::vector<std::string> splitString(const std::string& str, char delimiter);
		static std::string trim(const std::string& str);
		static bool isValidCommand(const std::string& command);

	private:
		// Private constructor - this is a utility class with static methods only
		CommandParser();
		CommandParser(const CommandParser& other);
		CommandParser& operator=(const CommandParser& other);
};

#endif
