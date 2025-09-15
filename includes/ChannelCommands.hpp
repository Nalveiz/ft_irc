/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ChannelCommands.hpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: soksak <soksak@42istanbul.com.tr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/11 00:00:00 by soksak            #+#    #+#             */
/*   Updated: 2025/09/14 23:25:42 by soksak           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CHANNELCOMMANDS_HPP
#define CHANNELCOMMANDS_HPP

#include "IRCMessage.hpp"
#include "IRCResponse.hpp"
#include <string>
#include <vector>
#include <algorithm>
#include <iostream>

class Server;
class Client;
class Channel;

class ChannelCommands
{
public:
	// Channel management commands
	static void handleJOIN(Server *server, Client *client, const IRCMessage &msg);
	static void handlePART(Server *server, Client *client, const IRCMessage &msg);
	static void handleKICK(Server *server, Client *client, const IRCMessage &msg);
	static void handleINVITE(Server *server, Client *client, const IRCMessage &msg);
	static void handleTOPIC(Server *server, Client *client, const IRCMessage &msg);

	// Helper function
	static bool validateBasicCommand(Server *server, Client *client, const IRCMessage &msg, const std::string &commandName);

private:
	ChannelCommands();
	ChannelCommands(const ChannelCommands &other);
	ChannelCommands &operator=(const ChannelCommands &other);
};

#endif // CHANNELCOMMANDS_HPP
