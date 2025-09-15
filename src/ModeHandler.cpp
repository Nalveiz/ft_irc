/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ModeHandler.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: soksak <soksak@42istanbul.com.tr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/14 23:18:03 by soksak            #+#    #+#             */
/*   Updated: 2025/09/14 23:18:04 by soksak           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/ModeHandler.hpp"
#include "../includes/Server.hpp"
#include "../includes/Client.hpp"
#include "../includes/Channel.hpp"
#include "../includes/CommandExecuter.hpp"

void ModeHandler::handleMODE(Server *server, Client *client, const IRCMessage &msg)
{
	if (!CommandExecuter::validateBasicCommand(server, client, msg, "MODE"))
		return;

	std::string target = msg.getParams()[0];

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

		if (msg.getParams().size() == 1)
		{
			std::string currentModes = getFullModeString(channel);
			std::string modeReply = IRCResponse::createModeReply(client->getNickname(), target, currentModes);
			client->writeAndEnablePollOut(server, modeReply);
			return;
		}

		std::string modeString = msg.getParams()[1];
		std::vector<std::string> params;
		for (size_t i = 2; i < msg.getParams().size(); ++i)
		{
			params.push_back(msg.getParams()[i]);
		}

		handleChannelMode(server, client, target, modeString, params);
	}
	else
	{
		client->writeAndEnablePollOut(server,
			IRCResponse::createUnknownModeFlag(client->getNickname()));
	}
}

void ModeHandler::handleChannelMode(Server *server, Client *client, std::string &channelName, std::string &modeString, std::vector<std::string> &params)
{
	Channel *channel = server->getChannel(channelName);
	if (!channel)
		return;

	if (!channel->isOperator(client->getClientFd()))
	{
		client->writeAndEnablePollOut(server,
			IRCResponse::createErrorChanOPrivsNeeded(client->getNickname(), channelName));
		return;
	}

	bool adding = true;
	size_t paramIndex = 0;
	std::string appliedModes;
	std::string modeParams;

	for (size_t i = 0; i < modeString.length(); ++i)
	{
		char c = modeString[i];

		if (c == '+')
		{
			adding = true;
			continue;
		}
		else if (c == '-')
		{
			adding = false;
			continue;
		}

		if (!isValidModeChar(c))
		{
			client->writeAndEnablePollOut(server,
				IRCResponse::createUnknownModeFlag(client->getNickname()));
			continue;
		}

		bool changed = false;
		switch (c)
		{
		case 'i':
			if (channel->isInviteOnly() != adding)
			{
				channel->setInviteOnly(adding);
				changed = true;
			}
			break;

		case 't':
			if (channel->isTopicRestricted() != adding)
			{
				channel->setTopicRestricted(adding);
				changed = true;
			}
			break;

		case 'k':
			if (adding && paramIndex < params.size())
			{
				channel->setKey(params[paramIndex]);
				if (!modeParams.empty())
					modeParams += " ";
				modeParams += params[paramIndex];
				paramIndex++;
				changed = true;
			}
			else if (!adding && !channel->getKey().empty())
			{
				channel->setKey("");
				changed = true;
			}
			break;

		case 'l':
			if (adding && paramIndex < params.size())
			{
				int limit = atoi(params[paramIndex].c_str());
				if (limit > 0)
				{
					channel->setUserLimit(limit);
					if (!modeParams.empty())
						modeParams += " ";
					modeParams += params[paramIndex];
					paramIndex++;
					changed = true;
				}
			}
			else if (!adding && channel->getUserLimit() > 0)
			{
				channel->setUserLimit(0);
				changed = true;
			}
			break;

		case 'o':
			if (paramIndex < params.size())
			{
				Client *targetClient = server->getClientByNickname(params[paramIndex]);
				if (targetClient && channel->isUserInChannel(targetClient->getClientFd()))
				{
					if (adding)
					{
						channel->addOperator(targetClient);
					}
					else
					{
						channel->removeOperator(targetClient->getClientFd());
					}
					if (!modeParams.empty())
						modeParams += " ";
					modeParams += params[paramIndex];
					changed = true;
				}
				paramIndex++;
			}
			break;
		}

		if (changed)
		{
			appliedModes += (adding ? "+" : "-");
			appliedModes += c;
		}
	}

	if (!appliedModes.empty())
	{
		std::string modeMsg = IRCResponse::createModeChange(client->getNickname(), client->getUsername(), server->getHostname(), channelName, appliedModes + (!modeParams.empty() ? " " + modeParams : ""));
		channel->broadcast(modeMsg, server, -1);
	}
}

std::string ModeHandler::getCurrentModes(Channel *channel)
{
	std::string modes = "+";

	if (channel->isInviteOnly())
		modes += "i";
	if (channel->isTopicRestricted())
		modes += "t";
	if (!channel->getKey().empty())
		modes += "k";
	if (channel->getUserLimit() > 0)
		modes += "l";

	return modes;
}

std::string ModeHandler::getFullModeString(Channel *channel)
{
	std::string modes = "+";
	std::string modeParams = "";

	if (channel->isInviteOnly())
		modes += "i";
	if (channel->isTopicRestricted())
		modes += "t";
	if (!channel->getKey().empty())
	{
		modes += "k";
		if (!modeParams.empty())
			modeParams += " ";
		modeParams += channel->getKey();
	}
	if (channel->getUserLimit() > 0)
	{
		modes += "l";
		if (!modeParams.empty())
			modeParams += " ";
		std::ostringstream oss;
		oss << channel->getUserLimit();
		modeParams += oss.str();
	}

	std::string fullModes = modes;
	if (!modeParams.empty())
	{
		fullModes += " ";
		fullModes += modeParams;
	}
	return fullModes;
}

bool ModeHandler::isValidModeChar(char mode)
{
	return mode == 'i' || mode == 't' || mode == 'k' || mode == 'o' || mode == 'l';
}
