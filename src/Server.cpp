#include "../includes/Server.hpp"

Server::Server(int &port, const std::string &password) : port(port), password(password)
{
	std::cout << "Server initializing..." << std::endl;
	// Create a socket, first things first
	this->serverSocket = socket(AF_INET, SOCK_STREAM, 0);

	if (this->serverSocket < 0)
	{
		throw SocketCreationFailed();
	}

	// Set socket options to reuse address
	int opt = 1;
	if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
	{
		throw SocketCreationFailed();
	}

	this->serverAddress.sin_family = AF_INET;
	this->serverAddress.sin_port = htons(this->port);
	this->serverAddress.sin_addr.s_addr = INADDR_ANY;

	// Add server socket to poll_fds
	pollfd server_pollfd;
	server_pollfd.fd = serverSocket;
	server_pollfd.events = POLLIN;
	server_pollfd.revents = 0;
	poll_fds.push_back(server_pollfd);
}

void Server::bindAndListen()
{
	// Bind the socket to the address and port, this is important.
	if (bind(this->serverSocket, (struct sockaddr *)&this->serverAddress, sizeof(this->serverAddress)) < 0)
	{
		throw SocketBindFailed();
	}
	// listen is gonna change this is the test..
	if (listen(this->serverSocket, 5) < 0)
	{
		throw SocketListenFailed();
	}
	std::cout << "Server is listening on port " << this->port << std::endl;
}

void Server::runServer()
{
	while (true)
	{
		// Use poll to wait for events
		int poll_count = poll(&poll_fds[0], poll_fds.size(), -1);

		if (poll_count < 0)
		{
			std::cerr << "Poll error" << std::endl;
			break;
		}

		// Check all file descriptors
		for (size_t i = 0; i < poll_fds.size(); ++i)
		{
			if (poll_fds[i].revents & POLLIN)
			{
				if (poll_fds[i].fd == serverSocket)
				{
					// New client connection
					int client_fd = accept(serverSocket, NULL, NULL);
					if (client_fd >= 0)
					{
						addClient(client_fd);
					}
				}
				else
				{
					// Existing client data
					handleClientData(poll_fds[i].fd);
				}
			}
		}
	}
}

void Server::addClient(int client_fd)
{
	Client* newClient = new Client(client_fd);
	clients[client_fd] = newClient;

	// Add to poll_fds
	pollfd client_pollfd;
	client_pollfd.fd = client_fd;
	client_pollfd.events = POLLIN;
	client_pollfd.revents = 0;
	poll_fds.push_back(client_pollfd);

	std::cout << "New client connected: " << client_fd << std::endl;
}

void Server::removeClient(int client_fd)
{
	// Remove from clients map
	std::map<int, Client*>::iterator it = clients.find(client_fd);
	if (it != clients.end())
	{
		delete it->second;
		clients.erase(it);
	}

	// Remove from poll_fds
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

void Server::handleClientData(int client_fd)
{
	char buffer[1024];
	ssize_t bytes_read = recv(client_fd, buffer, sizeof(buffer) - 1, 0);

	if (bytes_read <= 0)
	{
		// Client disconnected
		removeClient(client_fd);
		return;
	}

	buffer[bytes_read] = '\0';
	std::cout << "Received from client " << client_fd << ": " << buffer << std::endl;

	// Find the client
	std::map<int, Client*>::iterator it = clients.find(client_fd);
	if (it != clients.end())
	{
		Client* client = it->second;
		client->appendToReadBuffer(std::string(buffer));

		// Process complete IRC messages (ending with \r\n)
		std::string& readBuffer = client->getReadBuffer();
		size_t pos = 0;

		while ((pos = readBuffer.find("\r\n")) != std::string::npos)
		{
			std::string message = readBuffer.substr(0, pos);
			readBuffer.erase(0, pos + 2);

			if (!message.empty())
			{
				// Parse and execute IRC command
				IRCMessage ircMsg = CommandParser::parseMessage(message);
				Commands::executeCommand(this, client, ircMsg);
			}
		}
	}
}

void Server::sendToClient(int client_fd, const std::string &message)
{
	send(client_fd, message.c_str(), message.length(), 0);
}

Server::~Server()
{
	// Clean up all clients
	for (std::map<int, Client*>::iterator it = clients.begin(); it != clients.end(); ++it)
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

const std::string& Server::getPassword() const
{
	return this->password;
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
