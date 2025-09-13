/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Chanell.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: soksak <soksak@42istanbul.com.tr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/04 00:19:13 by soksak            #+#    #+#             */
/*   Updated: 2025/09/13 20:08:00 by soksak           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Channel.hpp"
#include "../includes/Server.hpp"

Channel::Channel(const std::string &name) : _name(name), _topic(""), _key(""), _userLimit(0), _inviteOnly(false), _topicRestricted(true)
{
	std::cout << "Channel " << _name << " created" << std::endl;
}

Channel::~Channel()
{
	std::cout << "Channel " << _name << " destroyed" << std::endl;
}

const std::string &Channel::getName() const
{
	return _name;
}

const std::string &Channel::getTopic() const
{
	return _topic;
}

size_t Channel::getUserCount() const
{
	return _users.size();
}

bool Channel::addUser(Client *user)
{
	if (!user)
		return false;

	int fd = user->getClientFd();
	_users[fd] = user;

	// If channel is empty, make first user an operator
	if (_users.size() == 1)
	{
		_operators[fd] = user;
		std::cout << "User " << user->getNickname() << " added to channel " << _name << " as operator" << std::endl;
	}
	else
		std::cout << "User " << user->getNickname() << " added to channel " << _name << std::endl;

	return true;
}

void Channel::removeUser(int fd)
{
	std::map<int, Client *>::iterator userIt = _users.find(fd);
	if (userIt != _users.end())
	{
		std::cout << "User " << userIt->second->getNickname() << " removed from channel " << _name << std::endl;
		_users.erase(userIt);
	}

	// Also remove from operators if they were one
	std::map<int, Client *>::iterator opIt = _operators.find(fd);
	if (opIt != _operators.end())
	{
		_operators.erase(opIt);
	}
}

bool Channel::isChannelEmpty() const
{
	std::map<int, Client *>::const_iterator it = _users.begin();
	if (it == _users.end())
		return true;
	return false;
}

bool Channel::isUserInChannel(int fd) const
{
	return _users.find(fd) != _users.end();
}

void Channel::addOperator(Client *user)
{
	if (!user)
		return;

	int fd = user->getClientFd();
	if (_users.find(fd) == _users.end())
		return; // User must be in channel first

	_operators[fd] = user;
	std::cout << "User " << user->getNickname() << " is now operator in channel " << _name << std::endl;
}

bool Channel::isOperator(int fd) const
{
	return _operators.find(fd) != _operators.end();
}

void Channel::setTopic(const std::string &topic)
{
	_topic = topic;
	std::cout << "Topic for channel " << _name << " set to: " << topic << std::endl;
}

void Channel::setKey(const std::string &key)
{
	_key = key;
}

void Channel::setUserLimit(size_t limit)
{
	_userLimit = limit;
}

void Channel::setInviteOnly(bool invite)
{
	_inviteOnly = invite;
}

void Channel::setTopicRestricted(bool restricted)
{
	_topicRestricted = restricted;
}

bool Channel::isInviteOnly() const
{
	return _inviteOnly;
}

bool Channel::isTopicRestricted() const
{
	return _topicRestricted;
}

const std::string &Channel::getKey() const
{
	return _key;
}

size_t Channel::getUserLimit() const
{
    return _userLimit;
}void Channel::removeOperator(int fd)
{
	_operators.erase(fd);
}

void Channel::inviteUser(int fd)
{
	_invited.insert(fd);
}

bool Channel::isUserInvited(int fd) const
{
	return _invited.find(fd) != _invited.end();
}

void Channel::removeInvite(int fd)
{
	_invited.erase(fd);
}

void Channel::broadcast(const std::string &message, Server *server, int exceptFd)
{
	for (std::map<int, Client *>::iterator it = _users.begin(); it != _users.end(); ++it)
	{
		if (it->first != exceptFd)
		{
			it->second->appendToSendBuffer(message);
			server->markClientForSending(it->first);
		}
	}
}

void Channel::sendUserList(Server *server, Client *client)
{
	std::string namesList = "";
	std::map<int, Client *> &users = server->getClients();

	// Build list of users in this channel
	for (std::map<int, Client *>::iterator it = users.begin(); it != users.end(); ++it)
	{
		if (isUserInChannel(it->first))
		{
			if (!namesList.empty())
				namesList += " ";

			// Add @ prefix for operators
			if (isOperator(it->first))
				namesList += "@";

			namesList += it->second->getNickname();
		}
	}

	// Send NAMES reply
	client->writeAndEnablePollOut(server, IRCResponse::createNamReply(client->getNickname(), _name, namesList));
	client->writeAndEnablePollOut(server, IRCResponse::createEndOfNames(client->getNickname(), _name));
}

bool Channel::isValidChannelName(const std::string &channelName)
{
	if (channelName.empty() || channelName[0] != '#')
		return false;

	if (channelName.length() < 2 || channelName.length() > 50)
		return false;

	// Check for invalid characters
	for (size_t i = 1; i < channelName.length(); ++i)
	{
		char c = channelName[i];
		if (c == ' ' || c == ',' || c == '\r' || c == '\n' || c == '\0')
			return false;
	}

	return true;
}
