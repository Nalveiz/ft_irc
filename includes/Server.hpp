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
#include <sstream>
#include "Client.hpp"
#include <fcntl.h>
#include "IRCMessage.hpp"
#include "CommandParser.hpp"
#include "CommandExecuter.hpp"

class Server
{
	private:
		int serverSocket;
		int port;
		std::string password;
		sockaddr_in serverAddress;
		std::map<int, Client*> clients;
		std::vector<pollfd> poll_fds;

		// Private copy constructor and assignment operator
		Server(const Server &other);
		Server &operator=(const Server &other);
		Server();
	public:
		// Constructor and Destructor
		Server(int &port, const std::string &password);
		~Server();

		// Main server methods
		void bindAndListen();
		void runServer();

		// Client management
		void setNonBlocking(int fd);
		void addClient(int client_fd);
		void removeClient(int client_fd);
		void handleClientData(pollfd &clientPfd);
		void sendToClient(int client_fd, const std::string &message);

		// Getters
		int getServerSocket() const;
		int getPort() const;
		const std::string& getPassword() const;

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

};

#endif
