/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: soksak <soksak@42istanbul.com.tr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/04 19:23:52 by soksak            #+#    #+#             */
/*   Updated: 2025/08/14 00:02:53 by soksak           ###   ########.fr       */
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
#include <poll.h>
#include <vector>

#include "../includes/User.hpp"

#define MAX_CLIENTS 10

class Server
{
	private:
		int serverSocket;
		int port;
		sockaddr_in serverAddress;

		// Poll file descriptors for clients
		pollfd pfds[MAX_CLIENTS + 1];
		int nfds; // Number of file descriptors in pfds

		// Users
		std::vector<User> users;

		// Disable copy constructor and assignment operator
		Server(const Server &other);
		Server &operator=(const Server &other);
		Server();
	public:
		// Constructor and Destructor
		Server(int &port);
		~Server();

		// Methods
		void acceptConnection();
		void bindAndListen();

		// Getters
		int getServerSocket() const;
		int getPort() const;

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

};

#endif
