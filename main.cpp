/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: soksak <soksak@42istanbul.com.tr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/04 19:47:09 by soksak            #+#    #+#             */
/*   Updated: 2025/08/04 20:02:02 by soksak           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "includes/Server.hpp"
#include <sstream>

int checkPort(const std::string &portStr)
{
	int port;
	std::string castedString(portStr);

	// Check if the port is a valid integer and within the range
	if (castedString.empty())
	{
		throw std::invalid_argument("Port cannot be empty.");
	}

	for (size_t i = 0; i < castedString.length(); ++i)
	{
		if (!isdigit(castedString[i]))
		{
			throw std::invalid_argument("Port must be a number.");
		}
	}

	// Hmm, now here i need to convert to int.
	std::stringstream ss(portStr);
	ss >> port;
	if (port < 1024 || port > 65535)
	{
		throw std::out_of_range("Port must be between 1024 and 65535.");
	}
	return port;
}

int main(int argc, char *argv[])
{
	if (argc != 2)
	{
		std::cerr << "Usage: " << argv[0] << " <port>" << std::endl;
		return 1;
	}

	try
	{
		int port = checkPort(argv[1]);
		Server server(port);
		server.bindAndListen();
		server.acceptConnection();
	}
	catch (const std::exception &e)
	{
		std::cerr << "Error: " << e.what() << std::endl;
		return 1;
	}

	return 0;
}
