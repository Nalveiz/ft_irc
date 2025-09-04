/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: soksak <soksak@42istanbul.com.tr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/04 00:19:22 by soksak            #+#    #+#             */
/*   Updated: 2025/09/04 00:19:22 by soksak           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CHANNEL_HPP
# define CHANNEL_HPP

# include <string>
# include <map>
# include <vector>
# include "Client.hpp"

class Channel {
private:
	std::string              _name;
	std::string              _topic;
	std::string              _key;        // channel password
	size_t                   _userLimit;
	bool                     _inviteOnly;
	std::map<int, Client*>   _users;      // fd → Client*
	std::map<int, Client*>   _operators;  // fd → Client*

public:
	Channel(const std::string &name);
	~Channel();

	// Getters
	const std::string &getName() const;
	const std::string &getTopic() const;

	// User management
	bool addUser(Client *user);
	void removeUser(int fd);
	bool isUserInChannel(int fd) const;

	// Operator management
	void addOperator(Client *user);
	bool isOperator(int fd) const;

	// Topic management
	void setTopic(const std::string &topic);

	// Modes
	void setKey(const std::string &key);
	void setUserLimit(size_t limit);
	void setInviteOnly(bool invite);

	// Broadcast
	void broadcast(const std::string &message, int exceptFd = -1);
};

#endif
