/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IRCMessage.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: soksak <soksak@42istanbul.com.tr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/07 18:07:23 by soksak            #+#    #+#             */
/*   Updated: 2025/09/15 02:17:14 by soksak           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef IRCMESSAGE_HPP
#define IRCMESSAGE_HPP

#include <string>
#include <vector>

class IRCMessage
{
	private:
		std::string command;
		std::vector<std::string> params;
		std::string trailing;
	public:
		IRCMessage();
		IRCMessage(const IRCMessage &other);
		IRCMessage &operator=(const IRCMessage &other);
		~IRCMessage();

		// Getters
		const std::string &getPrefix() const;
		const std::string &getCommand() const;
		const std::string &getTrailing() const;
		const std::vector<std::string> &getParams() const;

		// Setter
		void setPrefix(const std::string &prefix);
		void setCommand(const std::string &command);
		void setTrailing(const std::string &trailing);
		void addParam(const std::string &param);
};

#endif
