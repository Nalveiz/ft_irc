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
#include <string>
#include <vector>
#include <algorithm>
#include <sys/socket.h>
#include <cctype>
#include <iostream>
#include <map>

// Forward declarations to avoid circular dependencies
class Server;
class Client;
class Channel;


/**
 * CommandExecuter - IRC Command Processing Engine
 *
 * This class handles the execution of IRC commands sent by clients.
 * It provides static methods for each IRC command and manages the
 * interaction between clients, channels, and the server.
 *
 * Features:
 * - Command parsing and validation
 * - User authentication (PASS, NICK, USER)
 * - Channel management (JOIN, PART)
 * - Message handling (PRIVMSG)
 * - Server utilities (PING, QUIT)
 */
class CommandExecuter
{
    public:
        // Main command dispatcher
        static void executeCommand(Server* server, Client* client, const IRCMessage& msg);

        // Authentication and registration commands
        static void handleCAP(int client_fd, const IRCMessage& message, Client& client);
        static void handlePASS(Server* server, Client* client, const IRCMessage& msg);
        static void handleNICK(Server* server, Client* client, const IRCMessage& msg);
        static void handleUSER(Server* server, Client* client, const IRCMessage& msg);

        // Channel management commands
        static void handleJOIN(Server* server, Client* client, const IRCMessage& msg);
        static void handlePART(Server* server, Client* client, const IRCMessage& msg);
        static void handleKICK(Server* server, Client* client, const IRCMessage& msg);
        static void handleINVITE(Server* server, Client* client, const IRCMessage& msg);
        static void handleTOPIC(Server* server, Client* client, const IRCMessage& msg);
        static void handleMODE(Server* server, Client* client, const IRCMessage& msg);

        // Communication commands
        static void handlePRIVMSG(Server* server, Client* client, const IRCMessage& msg);

        // Server utility commands
        static void handlePING(Server* server, Client* client, const IRCMessage& msg);
        static void handleQUIT(Server* server, Client* client, const IRCMessage& msg);

        // Helper functions
        static bool validateBasicCommand(Server* server, Client* client, const IRCMessage& msg, const std::string& commandName);

    private:
        // Private constructor - this is a utility class with static methods only
        CommandExecuter();
        CommandExecuter(const CommandExecuter& other);
        CommandExecuter& operator=(const CommandExecuter& other);
};

#endif // COMMANDEXECUTER_HPP
