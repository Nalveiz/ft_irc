/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Commands.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: soksak <soksak@42istanbul.com.tr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/14 00:00:00 by soksak            #+#    #+#             */
/*   Updated: 2025/08/14 00:00:00 by soksak           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef COMMANDS_HPP
#define COMMANDS_HPP

#include "Server.hpp"
#include "Client.hpp"
#include "IRCMessage.hpp"
#include <string>
#include <vector>

// Forward declarations
class Server;
class Client;


// Command handlers
class CommandExecuter
{
    public:
        static void executeCommand(Server* server, Client* client, const IRCMessage& msg);

        // IRC Commands
        static void handleCAP(int client_fd, const IRCMessage& message, Client& client);
        static void handlePASS(Server* server, Client* client, const IRCMessage& msg);
        static void handleNICK(Server* server, Client* client, const IRCMessage& msg);
        static void handleUSER(Server* server, Client* client, const IRCMessage& msg);
        static void handlePING(Server* server, Client* client, const IRCMessage& msg);
        static void handleQUIT(Server* server, Client* client, const IRCMessage& msg);

        // Helper functions
        static void sendWelcome(Server* server, Client* client);
        static void writeClientSendBuffer(Server* server, Client* client, const std::string& reply);
};


// IRC Numeric replies
#define RPL_WELCOME(nick) (std::string(":localhost 001 ") + nick + " :Welcome to the IRC Network " + nick + "!\r\n")
#define RPL_YOURHOST(nick) (std::string(":localhost 002 ") + nick + " :Your host is localhost, running version 1.0\r\n")
#define RPL_CREATED(nick) (std::string(":localhost 003 ") + nick + " :This server was created today\r\n")
#define RPL_MYINFO(nick) (std::string(":localhost 004 ") + "localhost 1.0 o o\r\n")

#define ERR_NEEDMOREPARAMS(nick, cmd) (std::string(":localhost 461 ") + nick + " " + cmd + " :Not enough parameters\r\n")
#define ERR_ALREADYREGISTRED(nick) (std::string(":localhost 462 ") + nick + " :You may not reregister\r\n")
#define ERR_PASSWDMISMATCH(nick) (std::string(":localhost 464 ") + nick + " :Password incorrect\r\n")
#define ERR_NICKNAMEINUSE(nick) (std::string(":localhost 433 * ") + nick + " :Nickname is already in use\r\n")

#endif
