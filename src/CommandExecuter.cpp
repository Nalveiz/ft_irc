/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CommandExecuter.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: soksak <soksak@42istanbul.com.tr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/07 19:25:45 by soksak            #+#    #+#             */
/*   Updated: 2025/09/07 23:20:57 by soksak           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/**
 * CommandExecuter - IRC Command Execution Engine
 *
 * This file contains the implementation of IRC command handlers.
 * Each IRC command (PASS, NICK, USER, etc.) has its own handler function.
 */

#include "../includes/CommandExecuter.hpp"
#include "../includes/Server.hpp"
#include "../includes/Client.hpp"
#include "../includes/Channel.hpp"
#include <algorithm>
#include <sys/socket.h>
#include <cctype>
#include <iostream>
#include <map>


/**
 * Execute IRC commands based on parsed message
 * This is the main dispatcher that routes commands to their handlers
 *
 * @param server Pointer to the server instance
 * @param client Pointer to the client that sent the command
 * @param msg Parsed IRC message containing command and parameters
 */
void CommandExecuter::executeCommand(Server* server, Client* client, const IRCMessage& msg)
{
    if (!server || !client)
    {
        std::cout << "Error: Invalid server or client pointer" << std::endl;
        return;
    }

    std::string cmd = msg.getCommand();

    // Convert command to uppercase for case-insensitive comparison
    for (size_t i = 0; i < cmd.length(); ++i)
    {
        cmd[i] = std::toupper(cmd[i]);
    }

    std::cout << "Executing command: " << cmd << " for client " << client->getClientFd() << std::endl;

    // Command routing - each command has its own handler
    if (cmd == "PASS")
        handlePASS(server, client, msg);
    else if (cmd == "NICK")
        handleNICK(server, client, msg);
    else if (cmd == "USER")
        handleUSER(server, client, msg);
    else if (cmd == "PING")
        handlePING(server, client, msg);
    else if (cmd == "QUIT")
        handleQUIT(server, client, msg);
    else if (cmd == "JOIN")
        handleJOIN(server, client, msg);
    else if (cmd == "PART")
        handlePART(server, client, msg);
    else if (cmd == "PRIVMSG")
        handlePRIVMSG(server, client, msg);
    else if (cmd == "CAP")
        handleCAP(client->getClientFd(), msg, *client);
    else
    {
        std::cout << "Unknown command: " << cmd << std::endl;
        // Could send ERR_UNKNOWNCOMMAND here
    }
}

// ...existing code...

/**
 * Handle CAP (Client Capability) command
 * This is used for capability negotiation with modern IRC clients
 *
 * @param client_fd Client file descriptor
 * @param message IRC message containing CAP subcommand
 * @param client Client object reference
 */
void CommandExecuter::handleCAP(int client_fd, const IRCMessage& message, Client& client)
{
    (void)client; // Unused parameter

    if (message.getParams().empty())
    {
        std::string response = ":localhost 410 * :Invalid CAP command\r\n";
        send(client_fd, response.c_str(), response.length(), 0);
        return;
    }

    std::string subcommand = message.getParams()[0];

    if (subcommand == "LS" || subcommand == "LIST")
    {
        // Send empty capability list (simple server implementation)
        std::string response = ":localhost CAP * LS :\r\n";
        send(client_fd, response.c_str(), response.length(), 0);
    }
    else if (subcommand == "REQ")
    {
        // No capabilities supported - send NAK
        std::string response = ":localhost CAP * NAK :\r\n";
        send(client_fd, response.c_str(), response.length(), 0);
    }
    else if (subcommand == "END")
    {
        // End capability negotiation - client will proceed with normal commands
        // No response needed
    }
    else
    {
        std::string response = ":localhost 410 * :Invalid CAP subcommand\r\n";
        send(client_fd, response.c_str(), response.length(), 0);
    }
}

/**
 * Handle PASS command - Server password authentication
 * Must be sent before NICK/USER registration
 *
 * @param server Server instance
 * @param client Client attempting authentication
 * @param msg IRC message containing password
 */
void CommandExecuter::handlePASS(Server* server, Client* client, const IRCMessage& msg)
{
    // Cannot change password after registration
    if (client->isRegistered())
    {
        writeClientSendBuffer(server, client, IRCResponse::createErrorAlreadyRegistered(client->getNickname()));
        return;
    }

    // Password parameter is required
    if (msg.getParams().empty())
    {
        writeClientSendBuffer(server, client, IRCResponse::createErrorNeedMoreParams("*", "PASS"));
        return;
    }

    std::string password = msg.getParams()[0];

    // Verify password against server password
    if (password == server->getPassword())
    {
        client->setPassword(true);
        std::cout << "Client " << client->getClientFd() << " provided correct password" << std::endl;
    }
    else
    {
        writeClientSendBuffer(server, client, IRCResponse::createErrorPasswdMismatch("*"));
        std::cout << "Client " << client->getClientFd() << " provided wrong password" << std::endl;
    }
}

/**
 * Handle NICK command - Set user nickname
 * Used during registration and for nickname changes
 *
 * @param server Server instance
 * @param client Client setting nickname
 * @param msg IRC message containing new nickname
 */
void CommandExecuter::handleNICK(Server* server, Client* client, const IRCMessage& msg)
{
    // Nickname parameter is required
    if (msg.getParams().empty())
    {
        std::string currentNick = client->getNickname().empty() ? "*" : client->getNickname();
        writeClientSendBuffer(server, client, IRCResponse::createErrorNeedMoreParams(currentNick, "NICK"));
        return;
    }

    std::string newNick = msg.getParams()[0];

    // Validate nickname format (basic validation)
    if (!isValidNickname(newNick))
    {
        std::string currentNick = client->getNickname().empty() ? "*" : client->getNickname();
        writeClientSendBuffer(server, client, IRCResponse::createErrorErroneusNickname(currentNick, newNick));
        return;
    }

    // Check if nickname is already in use
    if (isNicknameInUse(server, newNick, client->getClientFd()))
    {
        std::string currentNick = client->getNickname().empty() ? "*" : client->getNickname();
        writeClientSendBuffer(server, client, IRCResponse::createErrorNicknameInUse(currentNick, newNick));
        return;
    }

    // Set the new nickname
    client->setNickname(newNick);
    std::cout << "Client " << client->getClientFd() << " set nickname to: " << newNick << std::endl;

    // Send welcome messages if client is now fully registered
    if (client->isRegistered())
    {
        sendWelcome(server, client);
    }
}

void CommandExecuter::handleUSER(Server* server, Client* client, const IRCMessage& msg)
{
    if (client->hasUser())
    {
        writeClientSendBuffer(server, client, IRCResponse::createErrorAlreadyRegistered(client->getNickname()));
        return;
    }

    if (msg.getParams().size() < 3 || msg.getTrailing().empty())
    {
        std::string currentNick = client->getNickname().empty() ? "*" : client->getNickname();
        writeClientSendBuffer(server, client, IRCResponse::createErrorNeedMoreParams(currentNick, "USER"));
        return;
    }

    std::string username = msg.getParams()[0];
    std::string realname = msg.getTrailing();

    client->setUsername(username);
    client->setRealname(realname);

    std::cout << "Client " << client->getClientFd() << " set username to: " << username << ", realname: " << realname << std::endl;

    // Send welcome if client is now fully registered
    if (client->isRegistered())
    {
        sendWelcome(server, client);
    }
}

void CommandExecuter::handlePING(Server* server, Client* client, const IRCMessage& msg)
{
    if (msg.getParams().empty())
    {
        writeClientSendBuffer(server, client, IRCResponse::createErrorNeedMoreParams(client->getNickname(), "PING"));
        return;
    }

    std::string pong_reply = IRCResponse::createPong("localhost", msg.getParams()[0]);
    writeClientSendBuffer(server, client, pong_reply);
}

void CommandExecuter::handleQUIT(Server* server, Client* client, const IRCMessage& msg)
{
    std::string quit_msg = msg.getTrailing().empty() ? "Client Quit" : msg.getTrailing();
    std::cout << "Client " << client->getClientFd() << " is quitting: " << quit_msg << std::endl;

    // TODO: Notify other clients in channels
    server->removeClient(client->getClientFd());
}

void CommandExecuter::sendWelcome(Server* server, Client* client)
{
    std::string nick = client->getNickname();
    std::string user = client->getUsername();
    std::string host = "localhost";

    writeClientSendBuffer(server, client, IRCResponse::createWelcome(nick, user, host));
    writeClientSendBuffer(server, client, IRCResponse::createYourHost(nick, "localhost"));
    writeClientSendBuffer(server, client, IRCResponse::createCreated(nick, "today"));
    writeClientSendBuffer(server, client, IRCResponse::createMyInfo(nick, "localhost"));

    std::cout << "Sent welcome messages to " << nick << std::endl;
}

/**
 * Write message to client's send buffer and mark for immediate sending
 *
 * @param server Server instance
 * @param client Target client
 * @param reply Message to send
 */
void CommandExecuter::writeClientSendBuffer(Server* server, Client* client, const std::string& reply)
{
	client->appendToSendBuffer(reply);
	server->markClientForSending(client->getClientFd());
}

/**
 * Validate IRC nickname format
 * RFC 2812: nicknames can contain letters, digits, and special chars [-_\\[\\]{}^`|]
 *
 * @param nickname Nickname to validate
 * @return true if valid, false otherwise
 */
bool CommandExecuter::isValidNickname(const std::string& nickname)
{
    if (nickname.empty() || nickname.length() > 9) // RFC 2812 max length
        return false;

    // First character must be a letter or special character
    char first = nickname[0];
    if (!((first >= 'A' && first <= 'Z') || (first >= 'a' && first <= 'z') ||
          first == '[' || first == ']' || first == '\\' || first == '`' ||
          first == '_' || first == '^' || first == '{' || first == '|' || first == '}'))
        return false;

    // Subsequent characters can include digits
    for (size_t i = 1; i < nickname.length(); ++i)
    {
        char c = nickname[i];
        if (!((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') ||
              (c >= '0' && c <= '9') ||
              c == '[' || c == ']' || c == '\\' || c == '`' ||
              c == '_' || c == '^' || c == '{' || c == '|' || c == '}' || c == '-'))
            return false;
    }

    return true;
}

/**
 * Check if nickname is already in use by another client
 *
 * @param server Server instance
 * @param nickname Nickname to check
 * @param excludeFd File descriptor to exclude from check (for nickname changes)
 * @return true if in use, false if available
 */
bool CommandExecuter::isNicknameInUse(Server* server, const std::string& nickname, int excludeFd)
{
    std::map<int, Client*>& clients = server->getClients();

    for (std::map<int, Client*>::iterator it = clients.begin(); it != clients.end(); ++it)
    {
        if (it->first != excludeFd && it->second->getNickname() == nickname)
            return true;
    }

    return false;
}

/**
 * Validate IRC channel name format
 * Channel names must start with # and contain valid characters
 *
 * @param channelName Channel name to validate
 * @return true if valid, false otherwise
 */
bool CommandExecuter::isValidChannelName(const std::string& channelName)
{
    if (channelName.empty() || channelName[0] != '#')
        return false;

    if (channelName.length() > 50) // Reasonable limit
        return false;

    for (size_t i = 1; i < channelName.length(); ++i)
    {
        char c = channelName[i];
        // Channel names cannot contain spaces, commas, or control characters
        if (c == ' ' || c == ',' || c == '\007' || c < 32)
            return false;
    }

    return true;
}

/**
 * Send channel user list (NAMES reply) to client
 * Shows all users currently in the channel with their status
 *
 * @param server Server instance
 * @param client Client requesting the list
 * @param channel Channel to list users for
 */
void CommandExecuter::sendChannelUserList(Server* server, Client* client, Channel* channel)
{
    std::string namesList = "";
    std::map<int, Client*>& users = server->getClients();

    // Build list of users in this channel
    for (std::map<int, Client*>::iterator it = users.begin(); it != users.end(); ++it)
    {
        if (channel->isUserInChannel(it->first))
        {
            if (!namesList.empty())
                namesList += " ";

            // Add @ prefix for operators
            if (channel->isOperator(it->first))
                namesList += "@";

            namesList += it->second->getNickname();
        }
    }

    // Send NAMES reply
    writeClientSendBuffer(server, client, createNamReplyMsg(client->getNickname(), channel->getName(), namesList));
    writeClientSendBuffer(server, client, createEndOfNamesMsg(client->getNickname(), channel->getName()));
}

// IRC Message Generator Functions
std::string CommandExecuter::createNamReplyMsg(const std::string& nick, const std::string& channel, const std::string& names)
{
    return ":localhost 353 " + nick + " = " + channel + " :" + names + "\r\n";
}

std::string CommandExecuter::createEndOfNamesMsg(const std::string& nick, const std::string& channel)
{
    return ":localhost 366 " + nick + " " + channel + " :End of /NAMES list\r\n";
}

std::string CommandExecuter::createPartMsg(const std::string& nick, const std::string& user, const std::string& host, const std::string& channel, const std::string& reason)
{
    return ":" + nick + "!" + user + "@" + host + " PART " + channel + " :" + reason + "\r\n";
}

/**
 * Handle JOIN command - Join a channel
 * Creates channel if it doesn't exist, adds user to channel
 *
 * @param server Server instance
 * @param client Client joining channel
 * @param msg IRC message containing channel name
 */
void CommandExecuter::handleJOIN(Server* server, Client* client, const IRCMessage& msg)
{
    // Must be registered to join channels
    if (!client->isRegistered())
    {
        writeClientSendBuffer(server, client, IRCResponse::createErrorNotRegistered(client->getNickname()));
        return;
    }

    // Channel name parameter is required
    if (msg.getParams().empty())
    {
        writeClientSendBuffer(server, client, IRCResponse::createErrorNeedMoreParams(client->getNickname(), "JOIN"));
        return;
    }

    std::string channelName = msg.getParams()[0];

    // Ensure channel name starts with #
    if (channelName[0] != '#')
    {
        channelName = "#" + channelName;
    }

    // Validate channel name format
    if (!isValidChannelName(channelName))
    {
        writeClientSendBuffer(server, client, IRCResponse::createErrorNoSuchChannel(client->getNickname(), channelName));
        return;
    }

    // Get existing channel or create new one
    Channel* channel = server->getChannel(channelName);
    if (!channel)
    {
        channel = server->createChannel(channelName);
        if (!channel)
        {
            std::cout << "Error: Failed to create channel " << channelName << std::endl;
            return;
        }
    }

    // Attempt to add user to channel
    if (channel->addUser(client))
    {
        // Send JOIN message to all users in channel (including the joining user)
        std::string joinMsg = IRCResponse::createJoin(client->getNickname(), client->getUsername(), "localhost", channelName);
        channel->broadcast(joinMsg, server);

        // Send channel user list (NAMES reply)
        sendChannelUserList(server, client, channel);
    }
    else
    {
        std::cout << "Failed to add user " << client->getNickname() << " to channel " << channelName << std::endl;
    }
}

void CommandExecuter::handlePART(Server* server, Client* client, const IRCMessage& msg)
{
    if (!client->isRegistered())
    {
        writeClientSendBuffer(server, client, IRCResponse::createErrorNotRegistered(client->getNickname()));
        return;
    }

    if (msg.getParams().empty())
    {
        writeClientSendBuffer(server, client, IRCResponse::createErrorNeedMoreParams(client->getNickname(), "PART"));
        return;
    }

    std::string channelName = msg.getParams()[0];
    Channel* channel = server->getChannel(channelName);

    if (!channel)
    {
        writeClientSendBuffer(server, client, IRCResponse::createErrorNoSuchChannel(client->getNickname(), channelName));
        return;
    }

    if (!channel->isUserInChannel(client->getClientFd()))
    {
        writeClientSendBuffer(server, client, IRCResponse::createErrorNotOnChannel(client->getNickname(), channelName));
        return;
    }

    std::string reason = msg.getTrailing().empty() ? "Leaving" : msg.getTrailing();

    // Send PART message to all users in channel
    std::string partMsg = IRCResponse::createPart(client->getNickname(), client->getUsername(), "localhost", channelName);
    channel->broadcast(partMsg, server);

    // Remove user from channel
    channel->removeUser(client->getClientFd());

    // If channel is empty, remove it
    bool isEmpty = true;
    std::map<int, Client*>& users = server->getClients();
    for (std::map<int, Client*>::iterator it = users.begin(); it != users.end(); ++it)
    {
        if (channel->isUserInChannel(it->first))
        {
            isEmpty = false;
            break;
        }
    }

    if (isEmpty)
    {
        server->removeChannel(channelName);
    }
}

void CommandExecuter::handlePRIVMSG(Server* server, Client* client, const IRCMessage& msg)
{
    if (!client->isRegistered())
    {
        writeClientSendBuffer(server, client, IRCResponse::createErrorNotRegistered(client->getNickname()));
        return;
    }

    if (msg.getParams().empty() || msg.getTrailing().empty())
    {
        writeClientSendBuffer(server, client, IRCResponse::createErrorNeedMoreParams(client->getNickname(), "PRIVMSG"));
        return;
    }

    std::string target = msg.getParams()[0];
    std::string message = msg.getTrailing();

    // Create PRIVMSG format: :nick!user@host PRIVMSG target :message
    std::string privmsgFormat = ":" + client->getNickname() + "!" + client->getUsername() + "@localhost PRIVMSG " + target + " :" + message + "\r\n";

    // Check if target is a channel (starts with #)
    if (target[0] == '#')
    {
        Channel* channel = server->getChannel(target);
        if (!channel)
        {
            writeClientSendBuffer(server, client, IRCResponse::createErrorNoSuchChannel(client->getNickname(), target));
            return;
        }

        if (!channel->isUserInChannel(client->getClientFd()))
        {
            writeClientSendBuffer(server, client, IRCResponse::createErrorNotOnChannel(client->getNickname(), target));
            return;
        }

        // Broadcast to all users in channel except sender
        channel->broadcast(privmsgFormat, server, client->getClientFd());
    }
    else
    {
        // Private message to user
        Client* targetClient = server->getClientByNickname(target);
        if (!targetClient)
        {
            writeClientSendBuffer(server, client, IRCResponse::createErrorNoSuchNick(client->getNickname(), target));
            return;
        }

        // Send message to target user
        targetClient->appendToSendBuffer(privmsgFormat);
        server->markClientForSending(targetClient->getClientFd());
    }
}
