/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CommandExecuter.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: soksak <soksak@42istanbul.com.tr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/07 19:25:45 by soksak            #+#    #+#             */
/*   Updated: 2025/09/10 00:07:52 by soksak           ###   ########.fr       */
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
#include "../includes/ModeHandler.hpp"

/**
 * Execute IRC commands based on parsed message
 * This is the main dispatcher that routes commands to their handlers
 *
 * @param server Pointer to the server instance
 * @param client Pointer to the client that sent the command
 * @param msg Parsed IRC message containing command and parameters
 */
void CommandExecuter::executeCommand(Server *server, Client *client, const IRCMessage &msg)
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
	else if (cmd == "KICK")
		handleKICK(server, client, msg);
	else if (cmd == "INVITE")
		handleINVITE(server, client, msg);
	else if (cmd == "TOPIC")
		handleTOPIC(server, client, msg);
	else if (cmd == "MODE")
		handleMODE(server, client, msg);
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
void CommandExecuter::handleCAP(int client_fd, const IRCMessage &message, Client &client)
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
void CommandExecuter::handlePASS(Server *server, Client *client, const IRCMessage &msg)
{
	// Cannot change password after registration
	if (client->isRegistered())
	{
		client->writeAndEnablePollOut(server, IRCResponse::createErrorAlreadyRegistered(client->getNickname()));
		return;
	}

	// Password parameter is required
	if (msg.getParams().empty())
	{
		client->writeAndEnablePollOut(server, IRCResponse::createErrorNeedMoreParams("*", "PASS"));
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
		client->writeAndEnablePollOut(server, IRCResponse::createErrorPasswdMismatch("*"));
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
void CommandExecuter::handleNICK(Server *server, Client *client, const IRCMessage &msg)
{
	// Nickname parameter is required
	if (msg.getParams().empty())
	{
		std::string currentNick = client->getNickname().empty() ? "*" : client->getNickname();
		client->writeAndEnablePollOut(server, IRCResponse::createErrorNeedMoreParams(currentNick, "NICK"));
		return;
	}

	std::string newNick = msg.getParams()[0];

	// Validate nickname format (basic validation)
	if (!Client::isValidNickname(newNick))
	{
		std::string currentNick = client->getNickname().empty() ? "*" : client->getNickname();
		client->writeAndEnablePollOut(server, IRCResponse::createErrorErroneusNickname(currentNick, newNick));
		return;
	}

	// Check if nickname is already in use
	if (Client::isNicknameInUse(server, newNick, client->getClientFd()))
	{
		std::string currentNick = client->getNickname().empty() ? "*" : client->getNickname();
		client->writeAndEnablePollOut(server, IRCResponse::createErrorNicknameInUse(currentNick, newNick));
		return;
	}

	// Set the new nickname
	client->setNickname(newNick);
	std::cout << "Client " << client->getClientFd() << " set nickname to: " << newNick << std::endl;

	// Send welcome messages if client is now fully registered
	if (client->isRegistered())
	{
		server->sendWelcome(client);
	}
}

void CommandExecuter::handleUSER(Server *server, Client *client, const IRCMessage &msg)
{
	if (client->hasUser())
	{
		client->writeAndEnablePollOut(server, IRCResponse::createErrorAlreadyRegistered(client->getNickname()));
		return;
	}

	if (msg.getParams().size() < 3 || msg.getTrailing().empty())
	{
		std::string currentNick = client->getNickname().empty() ? "*" : client->getNickname();
		client->writeAndEnablePollOut(server, IRCResponse::createErrorNeedMoreParams(currentNick, "USER"));
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
		server->sendWelcome(client);
	}
}

void CommandExecuter::handlePING(Server *server, Client *client, const IRCMessage &msg)
{
	if (msg.getParams().empty())
	{
		client->writeAndEnablePollOut(server, IRCResponse::createErrorNeedMoreParams(client->getNickname(), "PING"));
		return;
	}

	std::string pong_reply = IRCResponse::createPong("localhost", msg.getParams()[0]);
	client->writeAndEnablePollOut(server, pong_reply);
}

void CommandExecuter::handleQUIT(Server *server, Client *client, const IRCMessage &msg)
{
	std::string quit_msg = msg.getTrailing().empty() ? "Client Quit" : msg.getTrailing();
	std::cout << "Client " << client->getClientFd() << " is quitting: " << quit_msg << std::endl;

	// TODO: Notify other clients in channels
	server->removeClient(client->getClientFd());
}

/**
 * Validate basic command requirements (registration and parameters)
 *
 * @param server Server instance
 * @param client Client executing command
 * @param msg IRC message
 * @param commandName Name of the command for error messages
 * @return true if validation passes, false otherwise
 */
bool CommandExecuter::validateBasicCommand(Server *server, Client *client, const IRCMessage &msg, const std::string &commandName)
{
	if (!client->isRegistered())
	{
		client->writeAndEnablePollOut(server, IRCResponse::createErrorNotRegistered(client->getNickname()));
		return false;
	}

	if (msg.getParams().empty())
	{
		client->writeAndEnablePollOut(server, IRCResponse::createErrorNeedMoreParams(client->getNickname(), commandName));
		return false;
	}

	return true;
}

/**
 * Handle JOIN command - Join a channel
 * Creates channel if it doesn't exist, adds user to channel
 *
 * @param server Server instance
 * @param client Client joining channel
 * @param msg IRC message containing channel name
 */
void CommandExecuter::handleJOIN(Server *server, Client *client, const IRCMessage &msg)
{
	// Must be registered to join channels
	if (!client->isRegistered())
	{
		client->writeAndEnablePollOut(server, IRCResponse::createErrorNotRegistered(client->getNickname()));
		return;
	}

	// Channel name parameter is required
	if (msg.getParams().empty())
	{
		client->writeAndEnablePollOut(server, IRCResponse::createErrorNeedMoreParams(client->getNickname(), "JOIN"));
		return;
	}

	std::string channelName = msg.getParams()[0];

	// Ensure channel name starts with #
	if (channelName[0] != '#')
	{
		channelName = "#" + channelName;
	}

	// Validate channel name format
	if (!Channel::isValidChannelName(channelName))
	{
		client->writeAndEnablePollOut(server, IRCResponse::createErrorNoSuchChannel(client->getNickname(), channelName));
		return;
	}

	// Get existing channel or create new one
	Channel *channel = server->getChannel(channelName);
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
		channel->broadcast(joinMsg, server, -1); // Send to all users in channel

		// Send channel user list (NAMES reply)
		channel->sendUserList(server, client);
	}
	else
	{
		std::cout << "Failed to add user " << client->getNickname() << " to channel " << channelName << std::endl;
	}
}

void CommandExecuter::handlePART(Server *server, Client *client, const IRCMessage &msg)
{
	if (!client->isRegistered())
	{
		client->writeAndEnablePollOut(server, IRCResponse::createErrorNotRegistered(client->getNickname()));
		return;
	}

	if (msg.getParams().empty())
	{
		client->writeAndEnablePollOut(server, IRCResponse::createErrorNeedMoreParams(client->getNickname(), "PART"));
		return;
	}

	std::string channelName = msg.getParams()[0];
	Channel *channel = server->getChannel(channelName);

	if (!channel)
	{
		client->writeAndEnablePollOut(server, IRCResponse::createErrorNoSuchChannel(client->getNickname(), channelName));
		return;
	}

	if (!channel->isUserInChannel(client->getClientFd()))
	{
		client->writeAndEnablePollOut(server, IRCResponse::createErrorNotOnChannel(client->getNickname(), channelName));
		return;
	}

	std::string reason = msg.getTrailing().empty() ? "Leaving" : msg.getTrailing();

	// Send PART message to all users in channel
	std::string partMsg = IRCResponse::createPart(client->getNickname(), client->getUsername(), "localhost", channelName);
	channel->broadcast(partMsg, server, -1); // Send to all users in channel

	// Remove user from channel
	channel->removeUser(client->getClientFd());

	// If channel is empty, remove it
	bool isEmpty = true;
	std::map<int, Client *> &users = server->getClients();
	for (std::map<int, Client *>::iterator it = users.begin(); it != users.end(); ++it)
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

void CommandExecuter::handleKICK(Server *server, Client *client, const IRCMessage &msg)
{
	if (!client->isRegistered())
	{
		client->writeAndEnablePollOut(server, IRCResponse::createErrorNotRegistered(client->getNickname()));
		return;
	}

	if (msg.getParams().size() < 2)
	{
		client->writeAndEnablePollOut(server, IRCResponse::createErrorNeedMoreParams(client->getNickname(), "KICK"));
		return;
	}

	std::string channelName = msg.getParams()[0];
	std::string targetNick = msg.getParams()[1];
	std::string reason = msg.getTrailing().empty() ? "No reason given" : msg.getTrailing();

	Channel *channel = server->getChannel(channelName);
	if (!channel)
	{
		client->writeAndEnablePollOut(server, IRCResponse::createErrorNoSuchChannel(client->getNickname(), channelName));
		return;
	}

	// Check if kicker is in the channel
	if (!channel->isUserInChannel(client->getClientFd()))
	{
		client->writeAndEnablePollOut(server, IRCResponse::createErrorNotOnChannel(client->getNickname(), channelName));
		return;
	}

	// Check if kicker is an operator
	if (!channel->isOperator(client->getClientFd()))
	{
		client->writeAndEnablePollOut(server, IRCResponse::createErrorChanOPrivsNeeded(client->getNickname(), channelName));
		return;
	}

	// Find target user
	Client *targetClient = server->getClientByNickname(targetNick);
	if (!targetClient)
	{
		client->writeAndEnablePollOut(server, IRCResponse::createErrorNoSuchNick(client->getNickname(), targetNick));
		return;
	}

	// Check if target is in the channel
	if (!channel->isUserInChannel(targetClient->getClientFd()))
	{
		client->writeAndEnablePollOut(server, IRCResponse::createErrorUserNotInChannel(client->getNickname(), targetNick, channelName));
		return;
	}

	// Send KICK message to all users in channel
	std::string kickMsg = IRCResponse::createKick(client->getNickname(), client->getUsername(), "localhost", channelName, targetNick, reason);
	channel->broadcast(kickMsg, server, -1); // Send to all users in channel

	// Remove target user from channel
	channel->removeUser(targetClient->getClientFd());

	// If channel is empty, remove it
	bool isEmpty = true;
	std::map<int, Client *> &users = server->getClients();
	for (std::map<int, Client *>::iterator it = users.begin(); it != users.end(); ++it)
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

void CommandExecuter::handleINVITE(Server *server, Client *client, const IRCMessage &msg)
{
	if (!client->isRegistered())
	{
		client->writeAndEnablePollOut(server, IRCResponse::createErrorNotRegistered(client->getNickname()));
		return;
	}

	if (msg.getParams().size() < 2)
	{
		client->writeAndEnablePollOut(server, IRCResponse::createErrorNeedMoreParams(client->getNickname(), "INVITE"));
		return;
	}

	std::string targetNick = msg.getParams()[0];
	std::string channelName = msg.getParams()[1];

	// Find target user
	Client *targetClient = server->getClientByNickname(targetNick);
	if (!targetClient)
	{
		client->writeAndEnablePollOut(server, IRCResponse::createErrorNoSuchNick(client->getNickname(), targetNick));
		return;
	}

	Channel *channel = server->getChannel(channelName);
	if (!channel)
	{
		client->writeAndEnablePollOut(server, IRCResponse::createErrorNoSuchChannel(client->getNickname(), channelName));
		return;
	}

	// Check if inviter is in the channel
	if (!channel->isUserInChannel(client->getClientFd()))
	{
		client->writeAndEnablePollOut(server, IRCResponse::createErrorNotOnChannel(client->getNickname(), channelName));
		return;
	}

	// Check if target is already in channel
	if (channel->isUserInChannel(targetClient->getClientFd()))
	{
		client->writeAndEnablePollOut(server, IRCResponse::createErrorUserOnChannel(client->getNickname(), targetNick, channelName));
		return;
	}

	// Check if channel is invite-only and user is not operator
	// For now, let's assume any user in channel can invite (we can make this stricter later)

	// Send INVITE message to target user
	std::string inviteMsg = IRCResponse::createInvite(client->getNickname(), client->getUsername(), "localhost", targetNick, channelName);
	targetClient->writeAndEnablePollOut(server, inviteMsg);

	// Send success confirmation to inviter
	client->writeAndEnablePollOut(server, IRCResponse::createInviting(client->getNickname(), targetNick, channelName));

	std::cout << "User " << client->getNickname() << " invited " << targetNick << " to channel " << channelName << std::endl;
}

/**
 * Handle TOPIC command - view or set channel topic
 *
 * TOPIC #channel [new_topic]
 *
 * If no new_topic is provided, shows current topic
 * If new_topic is provided, sets it (if user has permission)
 */
void CommandExecuter::handleTOPIC(Server *server, Client *client, const IRCMessage &msg)
{
	if (!validateBasicCommand(server, client, msg, "TOPIC"))
		return;

	const std::vector<std::string> &params = msg.getParams();
	std::string channelName = params[0];
	Channel *channel = server->getChannel(channelName);

	// Check if channel exists
	if (!channel)
	{
		client->writeAndEnablePollOut(server, IRCResponse::createErrorNoSuchChannel(client->getNickname(), channelName));
		return;
	}

	// Check if user is in channel
	if (!channel->isUserInChannel(client->getClientFd()))
	{
		client->writeAndEnablePollOut(server, IRCResponse::createErrorNotOnChannel(client->getNickname(), channelName));
		return;
	}

	// If no topic parameter provided, return current topic
	if (params.size() == 1)
	{
		const std::string &currentTopic = channel->getTopic();
		if (currentTopic.empty())
		{
			client->writeAndEnablePollOut(server, IRCResponse::createNoTopicReply(client->getNickname(), channelName));
		}
		else
		{
			client->writeAndEnablePollOut(server, IRCResponse::createTopicReply(client->getNickname(), channelName, currentTopic));
		}
		return;
	}

	// Setting topic - check permissions
	if (channel->isTopicRestricted() && !channel->isOperator(client->getClientFd()))
	{
		client->writeAndEnablePollOut(server, IRCResponse::createErrorChanOPrivsNeeded(client->getNickname(), channelName));
		return;
	}

	// Set the new topic
	std::string newTopic = params[1];
	channel->setTopic(newTopic);

	// Broadcast topic change to all users in channel
	std::string topicMsg = IRCResponse::createTopic(client->getNickname(), client->getUsername(), "localhost", channelName, newTopic);
	channel->broadcast(topicMsg, server, -1); // -1 means send to everyone including the sender

	std::cout << "Topic for channel " << channelName << " changed by " << client->getNickname() << " to: " << newTopic << std::endl;
}

/**
 * Handle MODE command - delegate to ModeHandler
 *
 * MODE #channel [modes] [parameters]
 * MODE nickname [modes]
 */
void CommandExecuter::handleMODE(Server *server, Client *client, const IRCMessage &msg)
{
	ModeHandler::handleMODE(server, client, msg);
}

void CommandExecuter::handlePRIVMSG(Server *server, Client *client, const IRCMessage &msg)
{
	if (!client->isRegistered())
	{
		client->writeAndEnablePollOut(server, IRCResponse::createErrorNotRegistered(client->getNickname()));
		return;
	}

	if (msg.getParams().empty() || msg.getTrailing().empty())
	{
		client->writeAndEnablePollOut(server, IRCResponse::createErrorNeedMoreParams(client->getNickname(), "PRIVMSG"));
		return;
	}

	std::string target = msg.getParams()[0];
	std::string message = msg.getTrailing();

	// Create PRIVMSG format: :nick!user@host PRIVMSG target :message
	std::string privmsgFormat = ":" + client->getNickname() + "!" + client->getUsername() + "@localhost PRIVMSG " + target + " :" + message + "\r\n";

	// Check if target is a channel (starts with #)
	if (target[0] == '#')
	{
		Channel *channel = server->getChannel(target);
		if (!channel)
		{
			client->writeAndEnablePollOut(server, IRCResponse::createErrorNoSuchChannel(client->getNickname(), target));
			return;
		}

		if (!channel->isUserInChannel(client->getClientFd()))
		{
			client->writeAndEnablePollOut(server, IRCResponse::createErrorNotOnChannel(client->getNickname(), target));
			return;
		}

		// Broadcast to all users in channel except sender
		channel->broadcast(privmsgFormat, server, client->getClientFd());
	}
	else
	{
		// Private message to user
		Client *targetClient = server->getClientByNickname(target);
		if (!targetClient)
		{
			client->writeAndEnablePollOut(server, IRCResponse::createErrorNoSuchNick(client->getNickname(), target));
			return;
		}

		// Send message to target user
		targetClient->writeAndEnablePollOut(server, privmsgFormat);
	}
}
