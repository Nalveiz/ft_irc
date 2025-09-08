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

        // Communication commands
        static void handlePRIVMSG(Server* server, Client* client, const IRCMessage& msg);

        // Server utility commands
        static void handlePING(Server* server, Client* client, const IRCMessage& msg);
        static void handleQUIT(Server* server, Client* client, const IRCMessage& msg);

        // Helper functions
        static void sendWelcome(Server* server, Client* client);
        static void writeClientSendBuffer(Server* server, Client* client, const std::string& reply);
        static bool isValidNickname(const std::string& nickname);
        static bool isNicknameInUse(Server* server, const std::string& nickname, int excludeFd = -1);
        static bool isValidChannelName(const std::string& channelName);
        static void sendChannelUserList(Server* server, Client* client, Channel* channel);

        // IRC message generators
        static std::string createNamReplyMsg(const std::string& nick, const std::string& channel, const std::string& names);
        static std::string createEndOfNamesMsg(const std::string& nick, const std::string& channel);
        static std::string createPartMsg(const std::string& nick, const std::string& user, const std::string& host, const std::string& channel, const std::string& reason);

    private:
        // Private constructor - this is a utility class with static methods only
        CommandExecuter();
        CommandExecuter(const CommandExecuter& other);
        CommandExecuter& operator=(const CommandExecuter& other);
};

#endif // COMMANDEXECUTER_HPP
