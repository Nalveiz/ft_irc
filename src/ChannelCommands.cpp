/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ChannelCommands.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: soksak <soksak@42istanbul.com.tr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/14 23:18:37 by soksak            #+#    #+#             */
/*   Updated: 2025/09/14 23:19:13 by soksak           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/ChannelCommands.hpp"
#include "../includes/Server.hpp"
#include "../includes/Client.hpp"
#include "../includes/Channel.hpp"
#include "../includes/ModeHandler.hpp"

bool ChannelCommands::validateBasicCommand(Server *server, Client *client, const IRCMessage &msg, const std::string &commandName)
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

void ChannelCommands::handleJOIN(Server *server, Client *client, const IRCMessage &msg)
{
	if (!validateBasicCommand(server, client, msg, "JOIN"))
		return;

	std::string channelName = msg.getParams()[0];

	if (channelName[0] != '#')
	{
		channelName = "#" + channelName;
	}

	if (!Channel::isValidChannelName(channelName))
	{
		client->writeAndEnablePollOut(server,
			IRCResponse::createErrorNoSuchChannel(client->getNickname(), channelName));
		return;
	}

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

	if (!channel->getKey().empty())
	{
		std::string providedKey;
		if (msg.getParams().size() > 1)
		{
			providedKey = msg.getParams()[1];
		}

		if (providedKey != channel->getKey())
		{
			client->writeAndEnablePollOut(server,
				IRCResponse::createErrorBadChannelKey(client->getNickname(), channelName));
			return;
		}
	}

	if (channel->isUserInChannel(client->getClientFd()))
	{
		client->writeAndEnablePollOut(server,
			IRCResponse::createErrorUserOnChannel(client->getNickname(), channelName));
		return;
	}

	if (channel->isInviteOnly() && !channel->isUserInvited(client->getClientFd()))
	{
		client->writeAndEnablePollOut(server,
			IRCResponse::createErrorInviteOnlyChannel(client->getNickname(), channelName));
		return;
	}

	if (channel->getUserLimit() > 0 && channel->getUserCount() >= channel->getUserLimit())
	{
		client->writeAndEnablePollOut(server,
			IRCResponse::createErrorChannelIsFull(client->getNickname(), channelName));
		return;
	}

	if (channel->addUser(client))
	{
		channel->removeInvite(client->getClientFd());

		channel->broadcast(IRCResponse::createJoin(client->getNickname(),
			client->getUsername(), server->getHostname(), channelName), server, -1);

		channel->sendUserList(server, client);

		std::string topic = channel->getTopic();
		if (!topic.empty())
			client->writeAndEnablePollOut(server,
				IRCResponse::createTopicReply(client->getNickname(), channelName, topic));
		else
			client->writeAndEnablePollOut(server,
				IRCResponse::createNoTopicReply(client->getNickname(), channelName));

		std::string fullModes = ModeHandler::getFullModeString(channel);
		client->writeAndEnablePollOut(server,
			IRCResponse::createModeReply(client->getNickname(), channelName, fullModes));
	}
	else
		std::cout << "Failed to add user " << client->getNickname() << " to channel " << channelName << std::endl;
}

void ChannelCommands::handlePART(Server *server, Client *client, const IRCMessage &msg)
{
	if (!validateBasicCommand(server, client, msg, "PART"))
		return;

	std::string channelName = msg.getParams()[0];
	Channel *channel = server->getChannel(channelName);

	if (!channel)
	{
		client->writeAndEnablePollOut(server,
			IRCResponse::createErrorNoSuchChannel(client->getNickname(), channelName));
		return;
	}

	if (!channel->isUserInChannel(client->getClientFd()))
	{
		client->writeAndEnablePollOut(server,
			IRCResponse::createErrorNotOnChannel(client->getNickname(), channelName));
		return;
	}

	std::string reason = msg.getTrailing().empty() ? "Leaving" : msg.getTrailing();

	std::string partMsg = IRCResponse::createPart(client->getNickname(), client->getUsername(), server->getHostname(), channelName);
	channel->broadcast(partMsg, server, -1);

	channel->removeUser(client->getClientFd());

	if (channel->isChannelEmpty())
	{
		server->removeChannel(channelName);
	}
}

void ChannelCommands::handleKICK(Server *server, Client *client, const IRCMessage &msg)
{
	if (!client->isRegistered())
	{
		client->writeAndEnablePollOut(server,
			IRCResponse::createErrorNotRegistered(client->getNickname()));
		return;
	}

	if (msg.getParams().size() < 2)
	{
		client->writeAndEnablePollOut(server,
			IRCResponse::createErrorNeedMoreParams(client->getNickname(), "KICK"));
		return;
	}

	std::string channelName = msg.getParams()[0];
	std::string targetNick = msg.getParams()[1];
	std::string reason = msg.getTrailing().empty() ? "No reason given" : msg.getTrailing();

	Channel *channel = server->getChannel(channelName);
	if (!channel)
	{
		client->writeAndEnablePollOut(server,
			IRCResponse::createErrorNoSuchChannel(client->getNickname(), channelName));
		return;
	}

	if (!channel->isUserInChannel(client->getClientFd()))
	{
		client->writeAndEnablePollOut(server,
			IRCResponse::createErrorNotOnChannel(client->getNickname(), channelName));
		return;
	}

	if (!channel->isOperator(client->getClientFd()))
	{
		client->writeAndEnablePollOut(server,
			IRCResponse::createErrorChanOPrivsNeeded(client->getNickname(), channelName));
		return;
	}

	Client *targetClient = server->getClientByNickname(targetNick);
	if (!targetClient)
	{
		client->writeAndEnablePollOut(server,
			IRCResponse::createErrorNoSuchNick(client->getNickname(), targetNick));
		return;
	}

	if (!channel->isUserInChannel(targetClient->getClientFd()))
	{
		client->writeAndEnablePollOut(server,
			IRCResponse::createErrorUserNotInChannel(client->getNickname(), targetNick, channelName));
		return;
	}

	std::string kickMsg = IRCResponse::createKick(client->getNickname(), client->getUsername(), server->getHostname(), channelName, targetNick, reason);
	channel->broadcast(kickMsg, server, -1);

	channel->removeUser(targetClient->getClientFd());

	if (channel->isChannelEmpty())
	{
		server->removeChannel(channelName);
	}

	std::cout << "User " << targetNick << " was kicked from " << channelName << " by " << client->getNickname() << std::endl;
}

void ChannelCommands::handleINVITE(Server *server, Client *client, const IRCMessage &msg)
{
	if (!client->isRegistered())
	{
		client->writeAndEnablePollOut(server,
			IRCResponse::createErrorNotRegistered(client->getNickname()));
		return;
	}

	if (msg.getParams().size() < 2)
	{
		client->writeAndEnablePollOut(server,
			IRCResponse::createErrorNeedMoreParams(client->getNickname(), "INVITE"));
		return;
	}

	std::string targetNick = msg.getParams()[0];
	std::string channelName = msg.getParams()[1];

	Client *targetClient = server->getClientByNickname(targetNick);
	if (!targetClient)
	{
		client->writeAndEnablePollOut(server,
			IRCResponse::createErrorNoSuchNick(client->getNickname(), targetNick));
		return;
	}

	Channel *channel = server->getChannel(channelName);
	if (!channel)
	{
		client->writeAndEnablePollOut(server,
			IRCResponse::createErrorNoSuchChannel(client->getNickname(), channelName));
		return;
	}

	if (!channel->isUserInChannel(client->getClientFd()))
	{
		client->writeAndEnablePollOut(server,
			IRCResponse::createErrorNotOnChannel(client->getNickname(), channelName));
		return;
	}

	if (channel->isUserInChannel(targetClient->getClientFd()))
	{
		client->writeAndEnablePollOut(server,
			IRCResponse::createErrorUserOnChannel(client->getNickname(), targetNick, channelName));
		return;
	}

	targetClient->writeAndEnablePollOut(server,
		IRCResponse::createInvite(client->getNickname(), client->getUsername(), server->getHostname(), targetNick, channelName));

	channel->inviteUser(targetClient->getClientFd());

	client->writeAndEnablePollOut(server,
		IRCResponse::createInviting(client->getNickname(), targetNick, channelName));

	std::cout << "User " << client->getNickname() << " invited " << targetNick << " to " << channelName << std::endl;
}

void ChannelCommands::handleTOPIC(Server *server, Client *client, const IRCMessage &msg)
{
	if (!validateBasicCommand(server, client, msg, "TOPIC"))
		return;

	std::string channelName = msg.getParams()[0];
	Channel *channel = server->getChannel(channelName);

	if (!channel)
	{
		client->writeAndEnablePollOut(server,
			IRCResponse::createErrorNoSuchChannel(client->getNickname(), channelName));
		return;
	}

	if (!channel->isUserInChannel(client->getClientFd()))
	{
		client->writeAndEnablePollOut(server,
			IRCResponse::createErrorNotOnChannel(client->getNickname(), channelName));
		return;
	}

	if (msg.getTrailing().empty())
	{
		std::string currentTopic = channel->getTopic();
		if (currentTopic.empty())
		{
			client->writeAndEnablePollOut(server,
				IRCResponse::createNoTopicReply(client->getNickname(), channelName));
		}
		else
		{
			client->writeAndEnablePollOut(server,
				IRCResponse::createTopicReply(client->getNickname(), channelName, currentTopic));
		}
		return;
	}

	if (channel->isTopicRestricted() && !channel->isOperator(client->getClientFd()))
	{
		client->writeAndEnablePollOut(server,
			IRCResponse::createErrorTopicOPrivsNeeded(client->getNickname(), channelName));
		return;
	}

	std::string newTopic = msg.getTrailing();
	channel->setTopic(newTopic);

	std::string topicMsg = IRCResponse::createTopic(client->getNickname(), client->getUsername(), server->getHostname(), channelName, newTopic);
	channel->broadcast(topicMsg, server, -1);

	std::cout << "Topic for " << channelName << " set to: " << newTopic << std::endl;
}
