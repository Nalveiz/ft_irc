/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IRCMessage.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: soksak <soksak@42istanbul.com.tr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/07 18:07:07 by soksak            #+#    #+#             */
/*   Updated: 2025/09/15 02:17:33 by soksak           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/IRCMessage.hpp"

IRCMessage::IRCMessage() : command(""), params(), trailing("") {}

IRCMessage::~IRCMessage()
{
}

IRCMessage::IRCMessage(const IRCMessage &other)
{
	this->command = other.command;
	this->params = other.params;
	this->trailing = other.trailing;
}

IRCMessage &IRCMessage::operator=(const IRCMessage &other)
{
	if (this != &other)
	{
		this->command = other.command;
		this->params = other.params;
		this->trailing = other.trailing;
	}
	return *this;
}

const std::string &IRCMessage::getCommand() const {
	return command;
}

const std::string &IRCMessage::getTrailing() const {
	return trailing;
}

const std::vector<std::string> &IRCMessage::getParams() const {
	return params;
}

void IRCMessage::setCommand(const std::string &command)
{
	this->command = command;
}

void IRCMessage::setTrailing(const std::string &trailing)
{
	this->trailing = trailing;
}

void IRCMessage::addParam(const std::string &param)
{
	params.push_back(param);
}
