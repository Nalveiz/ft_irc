/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   User.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: soksak <soksak@42istanbul.com.tr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/13 23:42:18 by soksak            #+#    #+#             */
/*   Updated: 2025/08/14 00:02:00 by soksak           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/User.hpp"

User::User(int &fd) : _fd(fd), _gotNick(false), _gotUser(false), _registered(false)
{
	std::cout << "User created with fd: " << fd << std::endl;
}

User::~User()
{
}

int User::fd() const
{
	return this->_fd;
}

const std::string &User::getNick() const
{
	return this->_nickName;
}

const std::string &User::getUser() const
{
	return this->_userName;
}

const std::string &User::getRealName() const
{
	return this->_realName;
}

void User::setNick(const std::string &n)
{
	this->_nickName = n;
	this->_gotNick = true;
}

void User::setUser(const std::string &u, const std::string &real)
{
	this->_userName = u;
	this->_realName = real;
	this->_gotUser = true;
}

bool User::hasNick() const
{
	return this->_gotNick;
}

bool User::hasUser() const
{
	return this->_gotUser;
}

bool User::readyToRegister() const
{
	return this->_gotNick && this->_gotUser;
}

bool User::isRegistered() const
{
	return this->_registered;
}

void User::markRegistered()
{
	if (this->readyToRegister())
	{
		this->_registered = true;
		std::cout << "User with fd " << this->_fd << " is now registered." << std::endl;
	}
	else
	{
		std::cerr << "User with fd " << this->_fd << " cannot be registered, missing nick or user." << std::endl;
	}
}
