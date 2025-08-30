/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: soksak <soksak@42istanbul.com.tr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/14 00:00:00 by soksak            #+#    #+#             */
/*   Updated: 2025/08/14 00:00:00 by soksak           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Client.hpp"

Client::Client(int client_fd) : _client_fd(client_fd), _isRegistered(false),
								_hasPassword(false), _hasNick(false), _hasUser(false)
{
	std::cout << "Client " << client_fd << " created." << std::endl;
}

Client::~Client()
{
	std::cout << "Client " << _client_fd << " destroyed." << std::endl;
}

// Getters
int Client::getClientFd() const
{
	return _client_fd;
}

const std::string& Client::getNickname() const
{
	return _nickname;
}

const std::string& Client::getUsername() const
{
	return _username;
}

const std::string& Client::getRealname() const
{
	return _realname;
}

std::string& Client::getReadBuffer()
{
	return _readBuffer;
}

std::string& Client::getSendBuffer()
{
	return _sendBuffer;
}

bool Client::isRegistered() const
{
	return _isRegistered;
}

bool Client::hasPassword() const
{
	return _hasPassword;
}

bool Client::hasNick() const
{
	return _hasNick;
}

bool Client::hasUser() const
{
	return _hasUser;
}

// Setters
void Client::setNickname(const std::string& nickname)
{
	_nickname = nickname;
	_hasNick = true;
	updateRegistrationStatus();
}

void Client::setUsername(const std::string& username)
{
	_username = username;
	_hasUser = true;
	updateRegistrationStatus();
}

void Client::setRealname(const std::string& realname)
{
	_realname = realname;
}

void Client::setPassword(bool has)
{
	_hasPassword = has;
	updateRegistrationStatus();
}

void Client::setRegistered(bool registered)
{
	_isRegistered = registered;
}

// Buffer operations
void Client::appendToReadBuffer(const std::string& data)
{
	_readBuffer += data;
}

void Client::appendToSendBuffer(const std::string& data)
{
	_sendBuffer += data;
}

void Client::clearReadBuffer()
{
	_readBuffer.clear();
}

void Client::clearSendBuffer()
{
	_sendBuffer.clear();
}

// Private helper function
void Client::updateRegistrationStatus()
{
	if (_hasPassword && _hasNick && _hasUser && !_isRegistered)
	{
		_isRegistered = true;
		std::cout << "Client " << _client_fd << " is now fully registered!" << std::endl;
	}
}
