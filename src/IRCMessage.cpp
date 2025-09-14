/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IRCMessage.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: soksak <soksak@42istanbul.com.tr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/07 18:07:07 by soksak            #+#    #+#             */
/*   Updated: 2025/09/14 23:18:23 by soksak           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/IRCMessage.hpp"

IRCMessage::IRCMessage() : prefix(""), command(""), params(), trailing("") {}

IRCMessage::~IRCMessage()
{
}

const std::string &IRCMessage::getPrefix() const {
	return prefix;
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

void IRCMessage::setPrefix(const std::string &prefix)
{
	this->prefix = prefix;
}

void IRCMessage::setTrailing(const std::string &trailing)
{
	this->trailing = trailing;
}

void IRCMessage::addParam(const std::string &param)
{
	params.push_back(param);
}
