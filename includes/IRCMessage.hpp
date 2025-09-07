/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IRCMessage.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: soksak <soksak@42istanbul.com.tr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/07 18:07:23 by soksak            #+#    #+#             */
/*   Updated: 2025/09/07 21:19:49 by soksak           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef IRCMESSAGE_HPP
#define IRCMESSAGE_HPP

#include <string>
#include <vector>

class IRCMessage
{
	private:
		std::string prefix;
		std::string command;
		std::vector<std::string> params;
		std::string trailing;
	public:
		IRCMessage();
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
