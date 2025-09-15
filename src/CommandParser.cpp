/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Commands.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: soksak <soksak@42istanbul.com.tr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/14 00:00:00 by soksak            #+#    #+#             */
/*   Updated: 2025/08/14 00:00:00 by soksak           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/CommandParser.hpp"

IRCMessage CommandParser::parseMessage(const std::string &message)
{
	IRCMessage msg;
	std::string line = trim(message);
	size_t pos = 0;

	if (line.empty())
	{
		std::cout << "Warning: Empty IRC message received" << std::endl;
		return msg;
	}

	if (line.length() > 510)
	{
		std::cout << "Warning: IRC message too long, truncating" << std::endl;
		line = line.substr(0, 510);
	}

	size_t trailing_pos = line.find(" :", pos);
	std::string params_part;

	if (trailing_pos != std::string::npos)
	{
		params_part = line.substr(pos, trailing_pos - pos);
		std::string trailing = line.substr(trailing_pos + 2);
		msg.setTrailing(trailing);
	}
	else
	{
		params_part = line.substr(pos);
	}

	std::vector<std::string> tokens = splitString(params_part, ' ');
	if (!tokens.empty())
	{
		std::string command = tokens[0];

		if (isValidCommand(command))
		{
			msg.setCommand(command);

			for (size_t i = 1; i < tokens.size() && i <= 15; ++i)
			{
				msg.addParam(tokens[i]);
			}
		}
		else
		{
			std::cout << "Warning: Invalid IRC command: " << command << std::endl;
		}
	}
	else
	{
		std::cout << "Warning: No command found in IRC message" << std::endl;
	}

	return msg;
}

std::vector<std::string> CommandParser::splitString(const std::string &str, char delimiter)
{
	std::vector<std::string> tokens;
	std::stringstream ss(str);
	std::string token;

	while (std::getline(ss, token, delimiter))
	{
		token = trim(token);
		if (!token.empty())
			tokens.push_back(token);
	}

	return tokens;
}

std::string CommandParser::trim(const std::string &str)
{
	if (str.empty())
		return str;

	size_t start = str.find_first_not_of(" \t\r\n");
	if (start == std::string::npos)
		return "";

	size_t end = str.find_last_not_of(" \t\r\n");
	return str.substr(start, end - start + 1);
}

bool CommandParser::isValidCommand(const std::string &command)
{
	if (command.empty())
		return false;

	for (size_t i = 0; i < command.length(); ++i)
	{
		char c = command[i];
		if (!((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') ||
			  (c >= '0' && c <= '9')))
		{
			return false;
		}
	}

	return true;
}
