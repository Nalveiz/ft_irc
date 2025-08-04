#include "../includes/Server.hpp"

Server::Server(int &port) : port(port)
{
	std::cout << "Server initializing..." << std::endl;
	// Create a socket, first things first
	this->serverSocket = socket(AF_INET, SOCK_STREAM, 0);

	if (this->serverSocket < 0)
	{
		throw SocketCreationFailed();
	}
	this->serverAddress.sin_family = AF_INET;
	this->serverAddress.sin_port = htons(this->port);
	this->serverAddress.sin_addr.s_addr = INADDR_ANY;
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

void Server::acceptConnection()
{
	int clientSocket = accept(this->serverSocket, NULL, NULL);
	if (clientSocket < 0)
	{
		throw SocketAcceptFailed();
	}
	std::cout << "Client connected." << std::endl;
}

Server::~Server()
{
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
