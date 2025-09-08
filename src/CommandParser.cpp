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
#include <iostream>


/**
 * Parse IRC message format: [PREFIX] COMMAND [params] [:trailing]
 * RFC 2812 compliant IRC message parsing
 *
 * @param message Raw IRC message string
 * @return Parsed IRCMessage object
 */
IRCMessage CommandParser::parseMessage(const std::string& message)
{
    IRCMessage msg;
    std::string line = trim(message);

    // Empty message handling
    if (line.empty())
    {
        std::cout << "Warning: Empty IRC message received" << std::endl;
        return msg;
    }

    // IRC message length limit check (RFC 2812: 512 bytes including CRLF)
    if (line.length() > 510)
    {
        std::cout << "Warning: IRC message too long, truncating" << std::endl;
        line = line.substr(0, 510);
    }

    size_t pos = 0;

    // Parse prefix (optional, starts with :)
    if (line[0] == ':')
    {
        size_t space = line.find(' ');
        if (space != std::string::npos && space > 1)
        {
            std::string prefix = line.substr(1, space - 1);
            msg.setPrefix(prefix);
            pos = space + 1;

            // Skip multiple spaces after prefix
            while (pos < line.length() && line[pos] == ' ')
                pos++;
        }
        else
        {
            std::cout << "Warning: Malformed prefix in IRC message" << std::endl;
            return msg; // Invalid format
        }
    }

    // Find trailing part (starts with " :")
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

    // Parse command and parameters
    std::vector<std::string> tokens = splitString(params_part, ' ');
    if (!tokens.empty())
    {
        std::string command = tokens[0];

        // Validate command name (IRC commands are alphanumeric)
        if (isValidCommand(command))
        {
            msg.setCommand(command);

            // Add parameters (max 15 parameters per RFC 2812)
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

/**
 * Split string by delimiter, removing empty tokens
 *
 * @param str String to split
 * @param delimiter Character to split by
 * @return Vector of non-empty tokens
 */
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

/**
 * Remove leading and trailing whitespace from string
 *
 * @param str String to trim
 * @return Trimmed string
 */
std::string CommandParser::trim(const std::string& str)
{
    if (str.empty())
        return str;

    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos)
        return "";

    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

/**
 * Validate IRC command name
 * IRC commands should be alphabetic characters only
 *
 * @param command Command string to validate
 * @return true if valid, false otherwise
 */
bool CommandParser::isValidCommand(const std::string& command)
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

