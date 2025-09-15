/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CommandExecuter.hpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: soksak <soksak@42istanbul.com.tr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/14 00:00:00 by soksak            #+#    #+#             */
/*   Updated: 2025/09/07 23:00:00 by soksak           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef COMMANDEXECUTER_HPP
#define COMMANDEXECUTER_HPP

#include "IRCMessage.hpp"
#include "IRCResponse.hpp"
#include "ChannelCommands.hpp"
#include <string>
#include <vector>
#include <algorithm>
#include <sys/socket.h>
#include <cctype>
#include <iostream>
#include <map>

class Server;
class Client;
class Channel;

class CommandExecuter
{
public:
	// Main command dispatcher
	static void executeCommand(Server *server, Client *client, const IRCMessage &msg);

	// Authentication and registration commands
	static void handlePASS(Server *server, Client *client, const IRCMessage &msg);
	static void handleNICK(Server *server, Client *client, const IRCMessage &msg);
	static void handleUSER(Server *server, Client *client, const IRCMessage &msg);

	// Communication commands
	static void handlePRIVMSG(Server *server, Client *client, const IRCMessage &msg);

	// Server utility commands
	static void handlePING(Server *server, Client *client, const IRCMessage &msg);
	static void handleQUIT(Server *server, Client *client, const IRCMessage &msg);
	static void handleDisconnection(Server *server, Client *client, const std::string message);

	// Helper functions
	static bool validateBasicCommand(Server *server, Client *client, const IRCMessage &msg, const std::string &commandName);

private:
	CommandExecuter();
	CommandExecuter(const CommandExecuter &other);
	CommandExecuter &operator=(const CommandExecuter &other);
};

#endif // COMMANDEXECUTER_HPP
