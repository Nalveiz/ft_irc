/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: soksak <soksak@42istanbul.com.tr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/04 19:23:52 by soksak            #+#    #+#             */
/*   Updated: 2025/08/04 19:41:33 by soksak           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
#define SERVER_HPP

#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <exception>
#include <vector>
#include <map>
#include <poll.h>
#include "Client.hpp"
#include <fcntl.h>
#include <csignal>
#include "IRCMessage.hpp"
#include "CommandParser.hpp"
#include "CommandExecuter.hpp"
#include "Channel.hpp"
#include "IRCResponse.hpp"

class Server
{
	private:
		int serverSocket;
		int port;
		std::string password;
		std::string hostname;
		sockaddr_in serverAddress;
		std::string creationTime;
		std::map<int, Client*> clients;
		std::map<std::string, Channel*> channels;
		std::vector<pollfd> poll_fds;

		// Signal handling
		static bool shouldStop;

		// Private validation methods
		void checkArgPort(const std::string &portStr);
		void checkArgPassword(const std::string &password);

		// Private copy constructor and assignment operator
		Server(const Server &other);
		Server &operator=(const Server &other);
		Server();
	public:
		// Constructor and Destructor
		Server(const std::string &portStr, const std::string &password, const std::string &hostname);
		~Server();

		// Main server methods
		void bindAndListen();
		void runServer();

		// Signal handling
		static void signalHandler(int sig);

		// Client management
		void setNonBlocking(int fd);
		void addClient(int client_fd);
		void removeClient(int client_fd);
		void handleClientData(pollfd &clientPfd);
		void sendToClient(pollfd &clientPfd, Client *client);
		void markClientForSending(int client_fd);

		// Getters
		int getServerSocket() const;
		int getPort() const;
		const std::string& getPassword() const;
		const std::string& getHostname() const;
		std::map<int, Client*>& getClients();
		std::map<std::string, Channel*>& getChannels();

		// Channel management
		Channel* createChannel(const std::string& name);
		Channel* getChannel(const std::string& name);
		void removeChannel(const std::string& name);
		Client* getClientByNickname(const std::string& nickname);

		// Client utilities
		void sendWelcome(Client* client);
		std::string getCurrentTime();

		// Exceptions
		class SocketCreationFailed : public std::exception
		{
			public:
				const char *what() const throw();
		};

		class SocketBindFailed : public std::exception
		{
			public:
				const char *what() const throw();
		};

		class SocketListenFailed : public std::exception
		{
			public:
				const char *what() const throw();
		};

		class SocketAcceptFailed : public std::exception
		{
			public:
				const char *what() const throw();
		};

		class NonBlockingFailed : public std::exception
		{
			public:
				const char *what() const throw();
		};

		class PollFailed : public std::exception
		{
			public:
				const char *what() const throw();
		};

		class InvalidPortNumber : public std::exception
		{
			public:
				const char *what() const throw();
		};

		class InvalidPassword : public std::exception
		{
			public:
				const char *what() const throw();
		};

};

#endif