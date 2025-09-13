/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: soksak <soksak@42istanbul.com.tr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/04 00:19:22 by soksak            #+#    #+#             */
/*   Updated: 2025/09/13 03:19:57 by soksak           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CHANNEL_HPP
# define CHANNEL_HPP

# include <string>
# include <map>
# include <vector>
# include <iostream>
# include <sys/socket.h>
# include "Client.hpp"
# include "IRCResponse.hpp"

// Forward declarations
class Server;

class Channel {
private:
	std::string              _name;
	std::string              _topic;
	std::string              _key;        // channel password
	size_t                   _userLimit;
	bool                     _inviteOnly;
	bool                     _topicRestricted;
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
	void removeOperator(int fd);

	// Topic management
	void setTopic(const std::string &topic);

	// Modes
	void setKey(const std::string &key);
	const std::string& getKey() const;
	void setUserLimit(size_t limit);
	size_t getUserLimit() const;
	void setInviteOnly(bool invite);
	bool isInviteOnly() const;
	void setTopicRestricted(bool restricted);
	bool isTopicRestricted() const;
	bool isChannelEmpty() const;

	// Broadcast
	void broadcast(const std::string &message, class Server* server, int exceptFd = -1);
	void sendUserList(class Server* server, class Client* client);

	// Static utility functions
	static bool isValidChannelName(const std::string& channelName);
};

#endif
