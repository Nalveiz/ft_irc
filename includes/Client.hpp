/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: soksak <soksak@42istanbul.com.tr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/14 00:00:00 by soksak            #+#    #+#             */
/*   Updated: 2025/08/14 00:00:00 by soksak           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>
#include <iostream>
#include <fcntl.h>
#include <cctype>
#include <map>

class Server;

class Client
{
	private:
		int			_client_fd;
		std::string	_nickname;
		std::string	_username;
		std::string	_realname;
		std::string	_readBuffer;
		std::string	_sendBuffer;
		bool		_isRegistered;
		bool		_hasPassword;
		bool		_hasNick;
		bool		_hasUser;

	public:
		// Constructor & Destructor
		Client(int client_fd);
		~Client();

		// Getters
		int getClientFd() const;
		const std::string& getNickname() const;
		const std::string& getUsername() const;
		const std::string& getRealname() const;
		std::string& getReadBuffer();
		std::string& getSendBuffer();
		bool isRegistered() const;
		bool hasPassword() const;
		bool hasNick() const;
		bool hasUser() const;

		// Setters
		void setNickname(const std::string& nickname);
		void setUsername(const std::string& username);
		void setRealname(const std::string& realname);
		void setPassword(bool has);
		void setRegistered(bool registered);

		// Buffer operations
		void appendToReadBuffer(const std::string& data);
		void appendToSendBuffer(const std::string& data);
		void clearReadBuffer();
		void clearSendBuffer();
		void writeAndEnablePollOut(class Server* server, const std::string& message);


		// Static utility functions
		static bool isValidNickname(const std::string& nickname);
		static bool isNicknameInUse(class Server* server, const std::string& nickname, int excludeFd = -1);

	private:
		Client(Client const &other);
		Client &operator=(Client const &other);
		void updateRegistrationStatus();
};

#endif
