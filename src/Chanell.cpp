/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Chanell.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: soksak <soksak@42istanbul.com.tr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/04 00:19:13 by soksak            #+#    #+#             */
/*   Updated: 2025/09/07 22:21:02 by soksak           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Channel.hpp"
#include "../includes/Server.hpp"
#include <sys/socket.h>
#include <iostream>

Channel::Channel(const std::string &name) : _name(name), _topic(""), _key(""), _userLimit(0), _inviteOnly(false)
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

bool Channel::addUser(Client *user)
{
    if (!user)
        return false;

    int fd = user->getClientFd();
    if (_users.find(fd) != _users.end())
        return false; // User already in channel

    // Check user limit
    if (_userLimit > 0 && _users.size() >= _userLimit)
        return false;

    _users[fd] = user;

    // If channel is empty, make first user an operator
    if (_users.size() == 1)
    {
        _operators[fd] = user;
        std::cout << "User " << user->getNickname() << " added to channel " << _name << " as operator" << std::endl;
    }
    else
    {
        std::cout << "User " << user->getNickname() << " added to channel " << _name << std::endl;
    }

    return true;
}

void Channel::removeUser(int fd)
{
    std::map<int, Client*>::iterator userIt = _users.find(fd);
    if (userIt != _users.end())
    {
        std::cout << "User " << userIt->second->getNickname() << " removed from channel " << _name << std::endl;
        _users.erase(userIt);
    }

    // Also remove from operators if they were one
    std::map<int, Client*>::iterator opIt = _operators.find(fd);
    if (opIt != _operators.end())
    {
        _operators.erase(opIt);
    }
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

void Channel::broadcast(const std::string &message, int exceptFd)
{
    for (std::map<int, Client*>::iterator it = _users.begin(); it != _users.end(); ++it)
    {
        if (it->first != exceptFd)
        {
            it->second->appendToSendBuffer(message);
        }
    }
}

void Channel::broadcast(const std::string &message, Server* server, int exceptFd)
{
    for (std::map<int, Client*>::iterator it = _users.begin(); it != _users.end(); ++it)
    {
        if (it->first != exceptFd)
        {
            it->second->appendToSendBuffer(message);
            server->markClientForSending(it->first);
        }
    }
}
