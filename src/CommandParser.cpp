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


// Parse IRC message format: [PREFIX] COMMAND [params] [:trailing]
IRCMessage CommandParser::parseMessage(const std::string& message)
{
    IRCMessage msg;
    std::string line = trim(message);

    if (line.empty())
        return msg;

    size_t pos = 0;

    // Check for prefix (starts with :)
    if (line[0] == ':')
    {
        size_t space = line.find(' ');
        if (space != std::string::npos)
        {
            msg.setPrefix(line.substr(1, space - 1));
            pos = space + 1;
        }
    }

    // Find trailing part (starts with :)
    size_t trailing_pos = line.find(" :", pos);
    std::string params_part;

    if (trailing_pos != std::string::npos)
    {
        params_part = line.substr(pos, trailing_pos - pos);
        msg.setTrailing(line.substr(trailing_pos + 2));
    }
    else
    {
        params_part = line.substr(pos);
    }

    // Parse command and parameters
    std::vector<std::string> tokens = splitString(params_part, ' ');
    if (!tokens.empty())
    {
        msg.setCommand(tokens[0]);
        for (size_t i = 1; i < tokens.size(); ++i)
        {
            msg.addParam(tokens[i]);
        }
    }

    return msg;
}

std::vector<std::string> CommandParser::splitString(const std::string& str, char delimiter)
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

std::string CommandParser::trim(const std::string& str)
{
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos)
        return "";

    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

