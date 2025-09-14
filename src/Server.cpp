/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: soksak <soksak@42istanbul.com.tr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/14 23:18:10 by soksak            #+#    #+#             */
/*   Updated: 2025/09/14 23:18:10 by soksak           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Server.hpp"

Server::Server(int &port, const std::string &password, const std::string &hostname) : port(port), password(password), hostname(hostname)
{
	std::cout << "Server initializing..." << std::endl;
	this->serverSocket = socket(AF_INET, SOCK_STREAM, 0);

	if (this->serverSocket < 0)
	{
		throw SocketCreationFailed();
	}

	creationTime = getCurrentTime();
	int opt = 1;
	if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
	{
		throw SocketCreationFailed();
	}

	this->serverAddress.sin_family = AF_INET;
	this->serverAddress.sin_port = htons(this->port);
	this->serverAddress.sin_addr.s_addr = INADDR_ANY;
	setNonBlocking(this->serverSocket);

	pollfd server_pollfd;
	server_pollfd.fd = serverSocket;
	server_pollfd.events = POLLIN;
	server_pollfd.revents = 0;
	poll_fds.push_back(server_pollfd);
}

void Server::bindAndListen()
{
	if (bind(this->serverSocket, (struct sockaddr *)&this->serverAddress, sizeof(this->serverAddress)) < 0)
	{
		throw SocketBindFailed();
	}
	if (listen(this->serverSocket, SOMAXCONN) < 0)
	{
		throw SocketListenFailed();
	}
	std::cout << "Server is listening on port " << this->port << std::endl;
}

void Server::runServer()
{
	while (true)
	{
		int poll_count = poll(&poll_fds[0], poll_fds.size(), -1);

		if (poll_count < 0)
		{
			std::cerr << "Poll error" << std::endl;
			break;
		}

		for (size_t i = 0; i < poll_fds.size(); ++i)
		{
			if (poll_fds[i].revents & POLLIN)
			{
				if (poll_fds[i].fd == serverSocket)
				{
					int client_fd = accept(serverSocket, NULL, NULL);
					if (client_fd >= 0)
						addClient(client_fd);
				}
				else
					handleClientData(poll_fds[i]);
			}
			if (poll_fds[i].revents & POLLOUT)
			{
				if (poll_fds[i].fd != serverSocket)
				{
					if (poll_fds[i].events & POLLOUT)
					{
						std::map<int, Client *>::iterator it = clients.find(poll_fds[i].fd);
						if (it != clients.end())
						{
							Client *client = it->second;
							std::string sendBuffer = client->getSendBuffer();
							if (!sendBuffer.empty())
							{
								std::cout << "Sending to client " << client->getClientFd() << ": " << sendBuffer << std::endl;
								sendToClient(poll_fds[i], client);
							}
						}
					}
				}
			}
		}
	}
}

void Server::setNonBlocking(int fd)
{
	if (fcntl(fd, F_SETFL, O_NONBLOCK) == -1)
	{
		throw NonBlockingFailed();
	}
}

void Server::addClient(int client_fd)
{
	try
	{
		setNonBlocking(client_fd);

		Client *newClient = new Client(client_fd);
		clients[client_fd] = newClient;

		pollfd client_pollfd;
		client_pollfd.fd = client_fd;
		client_pollfd.events = POLLIN;
		client_pollfd.revents = 0;
		poll_fds.push_back(client_pollfd);

		std::cout << "New client connected: " << client_fd << std::endl;
	}
	catch (const std::exception &e)
	{
		std::cerr << "Error adding client: " << client_fd << " - " << e.what() << '\n';
		return;
	}
}

void Server::removeClient(int client_fd)
{
	std::vector<std::string> channelsToRemove;

	for (std::map<std::string, Channel *>::iterator it = channels.begin(); it != channels.end(); ++it)
	{
		Channel *channel = it->second;
		if (channel && channel->isUserInChannel(client_fd))
		{
			channel->removeUser(client_fd);
			if (channel->isChannelEmpty())
			{
				channelsToRemove.push_back(channel->getName());
			}
		}
	}

	for (size_t i = 0; i < channelsToRemove.size(); ++i)
	{
		removeChannel(channelsToRemove[i]);
	}

	std::cout << "Removing client: " << client_fd << std::endl;

	std::map<int, Client *>::iterator it = clients.find(client_fd);
	if (it != clients.end())
	{
		delete it->second;
		clients.erase(it);
	}

	for (std::vector<pollfd>::iterator it = poll_fds.begin(); it != poll_fds.end(); ++it)
	{
		if (it->fd == client_fd)
		{
			poll_fds.erase(it);
			break;
		}
	}

	close(client_fd);
	std::cout << "Client disconnected: " << client_fd << std::endl;
}

void Server::handleClientData(pollfd &clientPfd)
{
	char buffer[4096];
	ssize_t bytes_read = recv(clientPfd.fd, buffer, sizeof(buffer) - 1, 0);

	if (bytes_read <= 0)
	{
		removeClient(clientPfd.fd);
		return;
	}

	buffer[bytes_read] = '\0';
	std::cout << "Received from client " << clientPfd.fd << ": " << buffer << std::endl;

	std::map<int, Client *>::iterator it = clients.find(clientPfd.fd);
	if (it != clients.end())
	{
		Client *client = it->second;
		client->appendToReadBuffer(std::string(buffer));
		std::string &readBuffer = client->getReadBuffer();
		size_t pos = 0;

		while ((pos = readBuffer.find("\r\n")) != std::string::npos)
		{
			std::string message = readBuffer.substr(0, pos);
			readBuffer.erase(0, pos + 2);

			if (!message.empty())
			{
				IRCMessage ircMsg = CommandParser::parseMessage(message);
				CommandExecuter::executeCommand(this, client, ircMsg);
			}
		}
	}
}

void Server::sendToClient(pollfd &clientPfd, Client *client)
{
	int bytes_sent = send(clientPfd.fd, client->getSendBuffer().c_str(), client->getSendBuffer().length(), 0);
	if (bytes_sent > 0)
	{
		client->getSendBuffer().erase(0, bytes_sent);
		if (client->getSendBuffer().empty())
			clientPfd.events &= ~POLLOUT;
	}
}

void Server::markClientForSending(int client_fd)
{
	for (size_t i = 0; i < poll_fds.size(); ++i)
	{
		if (poll_fds[i].fd == client_fd)
		{
			if (!(poll_fds[i].events & POLLOUT))
				poll_fds[i].events |= POLLOUT;
			break;
		}
	}
}

Server::~Server()
{
	for (std::map<std::string, Channel *>::iterator it = channels.begin(); it != channels.end(); ++it)
	{
		delete it->second;
	}
	channels.clear();

	for (std::map<int, Client *>::iterator it = clients.begin(); it != clients.end(); ++it)
	{
		delete it->second;
	}
	clients.clear();

	close(this->serverSocket);
	std::cout << "Server socket closed." << std::endl;
}

int Server::getServerSocket() const
{
	return this->serverSocket;
}

int Server::getPort() const
{
	return this->port;
}

const std::string &Server::getPassword() const
{
	return this->password;
}

const std::string &Server::getHostname() const
{
	return this->hostname;
}

std::map<int, Client *> &Server::getClients()
{
	return this->clients;
}

std::map<std::string, Channel *> &Server::getChannels()
{
	return this->channels;
}

Channel *Server::createChannel(const std::string &name)
{
	if (channels.find(name) != channels.end())
		return channels[name];

	Channel *newChannel = new Channel(name);
	channels[name] = newChannel;
	std::cout << "Channel " << name << " created" << std::endl;
	return newChannel;
}

Channel *Server::getChannel(const std::string &name)
{
	std::map<std::string, Channel *>::iterator it = channels.find(name);
	if (it != channels.end())
		return it->second;
	return NULL;
}

void Server::removeChannel(const std::string &name)
{
	std::map<std::string, Channel *>::iterator it = channels.find(name);
	if (it != channels.end())
	{
		delete it->second;
		channels.erase(it);
		std::cout << "Channel " << name << " removed" << std::endl;
	}
}

Client *Server::getClientByNickname(const std::string &nickname)
{
	for (std::map<int, Client *>::iterator it = clients.begin(); it != clients.end(); ++it)
	{
		if (it->second->getNickname() == nickname)
			return it->second;
	}
	return NULL;
}

std::string Server::getCurrentTime()
{
	time_t now = time(NULL);
	char buf[64];
	strftime(buf, sizeof(buf), "%c", localtime(&now));
	return std::string(buf);
}

void Server::sendWelcome(Client *client)
{
	client->writeAndEnablePollOut(this, IRCResponse::createWelcome(client->getNickname(), client->getUsername(), hostname));
	client->writeAndEnablePollOut(this, IRCResponse::createYourHost(client->getNickname(), hostname));
	client->writeAndEnablePollOut(this, IRCResponse::createCreated(client->getNickname(), creationTime));
	client->writeAndEnablePollOut(this, IRCResponse::createMyInfo(client->getNickname(), hostname));
	client->writeAndEnablePollOut(this, IRCResponse::createISupport(client->getNickname()));

	std::cout << "Sent welcome messages to " << client->getNickname() << std::endl;
}

const char *Server::SocketCreationFailed::what() const throw()
{
	return "Socket creation failed.";
}

const char *Server::SocketBindFailed::what() const throw()
{
	return "Socket bind failed.";
}

const char *Server::SocketListenFailed::what() const throw()
{
	return "Socket listen failed.";
}

const char *Server::SocketAcceptFailed::what() const throw()
{
	return "Socket accept failed.";
}

const char *Server::NonBlockingFailed::what() const throw()
{
	return "Setting non-blocking mode failed.";
}
