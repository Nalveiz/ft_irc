/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CommandExecuter.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: soksak <soksak@42istanbul.com.tr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/07 19:25:45 by soksak            #+#    #+#             */
/*   Updated: 2025/09/15 01:06:37 by soksak           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/CommandExecuter.hpp"
#include "../includes/Server.hpp"
#include "../includes/Client.hpp"
#include "../includes/Channel.hpp"
#include "../includes/ModeHandler.hpp"

void CommandExecuter::executeCommand(Server *server, Client *client, const IRCMessage &msg)
{
	if (!server || !client)
	{
		std::cout << "Error: Invalid server or client pointer" << std::endl;
		return;
	}

	std::string cmd = msg.getCommand();

	for (size_t i = 0; i < cmd.length(); ++i)
	{
		cmd[i] = std::toupper(cmd[i]);
	}

	std::cout << "Executing command: " << cmd << " for client " << client->getClientFd() << std::endl;

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
		ChannelCommands::handleJOIN(server, client, msg);
	else if (cmd == "PART")
		ChannelCommands::handlePART(server, client, msg);
	else if (cmd == "KICK")
		ChannelCommands::handleKICK(server, client, msg);
	else if (cmd == "INVITE")
		ChannelCommands::handleINVITE(server, client, msg);
	else if (cmd == "TOPIC")
		ChannelCommands::handleTOPIC(server, client, msg);
	else if (cmd == "MODE")
		ModeHandler::handleMODE(server, client, msg);
	else if (cmd == "PRIVMSG")
		handlePRIVMSG(server, client, msg);
	else
	{
		std::cout << "Unknown command: " << cmd << std::endl;
		client->writeAndEnablePollOut(server,
			IRCResponse::createErrorUnknownCommand(client->getNickname(), cmd));
	}
}

void CommandExecuter::handlePASS(Server *server, Client *client, const IRCMessage &msg)
{
	if (client->hasPassword())
	{
		client->writeAndEnablePollOut(server,
			IRCResponse::createErrorAlreadyRegistered(client->getNickname()));
		return;
	}

	if (msg.getParams().empty())
	{
		client->writeAndEnablePollOut(server,
			IRCResponse::createErrorNeedMoreParams("*", "PASS"));
		return;
	}

	std::string password = msg.getParams()[0];

	if (password == server->getPassword())
	{
		client->setPassword(true);
		std::cout << "Client " << client->getClientFd() << " provided correct password" << std::endl;
		client->writeAndEnablePollOut(server,
			IRCResponse::createNotice("*", "Password accepted"));
	}
	else
	{
		client->writeAndEnablePollOut(server,
			IRCResponse::createErrorPasswdMismatch("*"));
		std::cout << "Client " << client->getClientFd() << " provided wrong password" << std::endl;
	}
}

void CommandExecuter::handleNICK(Server *server, Client *client, const IRCMessage &msg)
{
	if (!client->hasPassword())
		return;

	if (msg.getParams().empty())
	{
		std::string currentNick = client->getNickname().empty() ? "*" : client->getNickname();
		client->writeAndEnablePollOut(server,
			IRCResponse::createErrorNeedMoreParams(currentNick, "NICK"));
		return;
	}

	std::string newNick = msg.getParams()[0];

	if (!Client::isValidNickname(newNick))
	{
		std::string currentNick = client->getNickname().empty() ? "*" : client->getNickname();
		client->writeAndEnablePollOut(server,
			IRCResponse::createErrorErroneusNickname(currentNick, newNick));
		return;
	}

	if (Client::isNicknameInUse(server, newNick, client->getClientFd()))
	{
		std::string currentNick = client->getNickname().empty() ? "*" : client->getNickname();
		client->writeAndEnablePollOut(server,
			IRCResponse::createErrorNicknameInUse(currentNick, newNick));
		return;
	}

	std::string oldNick = client->getNickname();

	client->setNickname(newNick);
	std::cout << "Client " << client->getClientFd() << " set nickname to: " << newNick << std::endl;

	if (!oldNick.empty())
	{
		std::map<std::string, Channel *> &channels = server->getChannels();
		for (std::map<std::string, Channel *>::iterator it = channels.begin(); it != channels.end(); ++it)
		{
			Channel *channel = it->second;
			if (channel && channel->isUserInChannel(client->getClientFd()))
			{
				channel->broadcast(IRCResponse::createNickChange(oldNick, client->getUsername(), server->getHostname(),
					 newNick), server, -1);
			}
		}
		return;
	}

	if (client->isRegistered())
		server->sendWelcome(client);
}

void CommandExecuter::handleUSER(Server *server, Client *client, const IRCMessage &msg)
{
	if (!client->hasPassword())
		return;

	if (client->hasUser())
	{
		client->writeAndEnablePollOut(server,
			IRCResponse::createErrorAlreadyRegistered(client->getNickname()));
		return;
	}

	if (msg.getParams().size() < 3 || msg.getTrailing().empty())
	{
		std::string currentNick = client->getNickname().empty() ? "*" : client->getNickname();
		client->writeAndEnablePollOut(server,
			IRCResponse::createErrorNeedMoreParams(currentNick, "USER"));
		return;
	}

	std::string username = msg.getParams()[0];
	std::string realname = msg.getTrailing();

	client->setUsername(username);
	client->setRealname(realname);

	std::cout << "Client " << client->getClientFd() << " set username to: " << username << ", realname: " << realname << std::endl;

	if (client->isRegistered())
	{
		server->sendWelcome(client);
	}
}

void CommandExecuter::handlePING(Server *server, Client *client, const IRCMessage &msg)
{
	if (!client->hasPassword())
		return;

	if (msg.getParams().empty())
	{
		client->writeAndEnablePollOut(server,
			IRCResponse::createErrorNeedMoreParams(client->getNickname(), "PING"));
		return;
	}
	client->writeAndEnablePollOut(server,
		IRCResponse::createPong(server->getHostname(), msg.getParams()[0]));
}

void CommandExecuter::handleQUIT(Server *server, Client *client, const IRCMessage &msg)
{
	std::string quit_msg = msg.getTrailing().empty() ? "Client Quit" : msg.getTrailing();
	std::cout << "Client " << client->getClientFd() << " is quitting: " << quit_msg << std::endl;

	if (!client->getNickname().empty())
	{
		std::map<std::string, Channel *> &channels = server->getChannels();
		for (std::map<std::string, Channel *>::iterator it = channels.begin(); it != channels.end(); ++it)
		{
			Channel *channel = it->second;
			if (channel && channel->isUserInChannel(client->getClientFd()))
			{
				channel->broadcast(IRCResponse::createQUIT(client->getNickname(), client->getUsername(),
					server->getHostname(), quit_msg), server, -1);
			}
		}
	}
	server->removeClient(client->getClientFd());
}

bool CommandExecuter::validateBasicCommand(Server *server, Client *client, const IRCMessage &msg, const std::string &commandName)
{
	if (!client->isRegistered())
	{
		client->writeAndEnablePollOut(server,
			IRCResponse::createErrorNotRegistered(client->getNickname()));
		return false;
	}

	if (msg.getParams().empty())
	{
		client->writeAndEnablePollOut(server,
			IRCResponse::createErrorNeedMoreParams(client->getNickname(), commandName));
		return false;
	}

	return true;
}

void CommandExecuter::handlePRIVMSG(Server *server, Client *client, const IRCMessage &msg)
{
	if (!client->isRegistered())
	{
		client->writeAndEnablePollOut(server,
			IRCResponse::createErrorNotRegistered(client->getNickname()));
		return;
	}

	if (msg.getParams().empty() || msg.getTrailing().empty())
	{
		client->writeAndEnablePollOut(server,
			IRCResponse::createErrorNeedMoreParams(client->getNickname(), "PRIVMSG"));
		return;
	}

	std::string target = msg.getParams()[0];
	std::string message = msg.getTrailing();

	std::string privmsgFormat = IRCResponse::createPrivmsg(client->getNickname(), client->getUsername(), server->getHostname(), target, message);

	if (target[0] == '#')
	{
		Channel *channel = server->getChannel(target);
		if (!channel)
		{
			client->writeAndEnablePollOut(server,
				IRCResponse::createErrorNoSuchChannel(client->getNickname(), target));
			return;
		}

		if (!channel->isUserInChannel(client->getClientFd()))
		{
			client->writeAndEnablePollOut(server,
				IRCResponse::createErrorNotOnChannel(client->getNickname(), target));
			return;
		}

		channel->broadcast(privmsgFormat, server, client->getClientFd());
	}
	else
	{
		Client *targetClient = server->getClientByNickname(target);
		if (!targetClient)
		{
			client->writeAndEnablePollOut(server,
				IRCResponse::createErrorNoSuchNick(client->getNickname(), target));
			return;
		}

		targetClient->writeAndEnablePollOut(server, privmsgFormat);
	}
}
