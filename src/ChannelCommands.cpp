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

	// Ensure channel name starts with #
	if (channelName[0] != '#')
	{
		channelName = "#" + channelName;
	}

	// Validate channel name format
	if (!Channel::isValidChannelName(channelName))
	{
		client->writeAndEnablePollOut(server,
			IRCResponse::createErrorNoSuchChannel(client->getNickname(), channelName));
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

	// Check channel key if channel has a password
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

	// Check if channel is invite only
	if (channel->isInviteOnly())
	{
		client->writeAndEnablePollOut(server,
			IRCResponse::createErrorInviteOnlyChannel(client->getNickname(), channelName));
		return;
	}

	// Check if user is already in the channel
	if (channel->isUserInChannel(client->getClientFd()))
	{
		// User is already in the channel - send appropriate message
		client->writeAndEnablePollOut(server,
			IRCResponse::createErrorUserOnChannel(client->getNickname(), channelName));
		return;
	}

	// Attempt to add user to channel
	if (channel->addUser(client))
	{
		// Send JOIN message to all users in channel (including the joining user)
		channel->broadcast(IRCResponse::createJoin(client->getNickname(),
			client->getUsername(), server->getHostname(), channelName), server, -1); // Send to all users in channel

		// Send channel user list (NAMES reply)
		channel->sendUserList(server, client);

		// Send channel topic to the joining user (if topic exists)
		std::string topic = channel->getTopic();
		if (!topic.empty())
		{
			client->writeAndEnablePollOut(server,
				IRCResponse::createTopicReply(client->getNickname(), channelName, topic));
		}
		else
		{
			client->writeAndEnablePollOut(server,
				IRCResponse::createNoTopicReply(client->getNickname(), channelName));
		}
	}
	else
	{
		std::cout << "Failed to add user " << client->getNickname() << " to channel " << channelName << std::endl;
	}
}

// Placeholder implementations - we'll copy the actual code from CommandExecuter.cpp
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

	// Send PART message to all users in channel
	std::string partMsg = IRCResponse::createPart(client->getNickname(), client->getUsername(), server->getHostname(), channelName);
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

	// Check if kicker is in the channel
	if (!channel->isUserInChannel(client->getClientFd()))
	{
		client->writeAndEnablePollOut(server,
			IRCResponse::createErrorNotOnChannel(client->getNickname(), channelName));
		return;
	}

	// Check if kicker is an operator
	if (!channel->isOperator(client->getClientFd()))
	{
		client->writeAndEnablePollOut(server,
			IRCResponse::createErrorChanOPrivsNeeded(client->getNickname(), channelName));
		return;
	}

	// Find target user
	Client *targetClient = server->getClientByNickname(targetNick);
	if (!targetClient)
	{
		client->writeAndEnablePollOut(server,
			IRCResponse::createErrorNoSuchNick(client->getNickname(), targetNick));
		return;
	}

	// Check if target is in the channel
	if (!channel->isUserInChannel(targetClient->getClientFd()))
	{
		client->writeAndEnablePollOut(server,
			IRCResponse::createErrorUserNotInChannel(client->getNickname(), targetNick, channelName));
		return;
	}

	// Send KICK message to all users in channel
	std::string kickMsg = IRCResponse::createKick(client->getNickname(), client->getUsername(), server->getHostname(), channelName, targetNick, reason);
	channel->broadcast(kickMsg, server, -1);

	// Remove target user from channel
	channel->removeUser(targetClient->getClientFd());

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

	// Find target user
	Client *targetClient = server->getClientByNickname(targetNick);
	if (!targetClient)
	{
		client->writeAndEnablePollOut(server,
			IRCResponse::createErrorNoSuchNick(client->getNickname(), targetNick));
		return;
	}

	// Check if channel exists
	Channel *channel = server->getChannel(channelName);
	if (!channel)
	{
		client->writeAndEnablePollOut(server,
			IRCResponse::createErrorNoSuchChannel(client->getNickname(), channelName));
		return;
	}

	// Check if inviter is in the channel
	if (!channel->isUserInChannel(client->getClientFd()))
	{
		client->writeAndEnablePollOut(server,
			IRCResponse::createErrorNotOnChannel(client->getNickname(), channelName));
		return;
	}

	// Check if target is already in channel
	if (channel->isUserInChannel(targetClient->getClientFd()))
	{
		client->writeAndEnablePollOut(server,
			IRCResponse::createErrorUserOnChannel(client->getNickname(), targetNick, channelName));
		return;
	}

	// Send invite message
	targetClient->writeAndEnablePollOut(server,
		IRCResponse::createInvite(client->getNickname(), client->getUsername(), server->getHostname(), targetNick, channelName));

	// Send confirmation to inviter
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

	// Check if user is in channel
	if (!channel->isUserInChannel(client->getClientFd()))
	{
		client->writeAndEnablePollOut(server,
			IRCResponse::createErrorNotOnChannel(client->getNickname(), channelName));
		return;
	}

	// If no trailing parameter, display current topic
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

	// Check if channel is +t (topic restricted) and user is not operator
	if (channel->isTopicRestricted() && !channel->isOperator(client->getClientFd()))
	{
		client->writeAndEnablePollOut(server,
			IRCResponse::createErrorTopicOPrivsNeeded(client->getNickname(), channelName));
		return;
	}

	// Set new topic
	std::string newTopic = msg.getTrailing();
	channel->setTopic(newTopic);

	// Broadcast topic change to all users in channel
	std::string topicMsg = IRCResponse::createTopic(client->getNickname(), client->getUsername(), server->getHostname(), channelName, newTopic);
	channel->broadcast(topicMsg, server, -1);

	std::cout << "Topic for " << channelName << " set to: " << newTopic << std::endl;
}

void ChannelCommands::handleMODE(Server *server, Client *client, const IRCMessage &msg)
{
	ModeHandler::handleMODE(server, client, msg);
}
