/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Commands.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: soksak <soksak@42istanbul.com.tr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/14 00:00:00 by soksak            #+#    #+#             */
/*   Updated: 2025/08/14 00:00:00 by soksak           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Commands.hpp"
#include "../includes/Server.hpp"
#include "../includes/Client.hpp"
#include <iostream>
#include <sstream>
#include <algorithm>

// Parse IRC message format: [PREFIX] COMMAND [params] [:trailing]
IRCMessage CommandParser::parseMessage(const std::string& message)
{
    IRCMessage msg;
    std::string line = trim(message);

    if (line.empty())
        return msg;

    size_t pos = 0;

    // Check for prefix (starts with :)
    if (line[0] == ':')
    {
        size_t space = line.find(' ');
        if (space != std::string::npos)
        {
            msg.prefix = line.substr(1, space - 1);
            pos = space + 1;
        }
    }

    // Find trailing part (starts with :)
    size_t trailing_pos = line.find(" :", pos);
    std::string params_part;

    if (trailing_pos != std::string::npos)
    {
        params_part = line.substr(pos, trailing_pos - pos);
        msg.trailing = line.substr(trailing_pos + 2);
    }
    else
    {
        params_part = line.substr(pos);
    }

    // Parse command and parameters
    std::vector<std::string> tokens = splitString(params_part, ' ');
    if (!tokens.empty())
    {
        msg.command = tokens[0];
        for (size_t i = 1; i < tokens.size(); ++i)
        {
            msg.params.push_back(tokens[i]);
        }
    }

    return msg;
}

std::vector<std::string> CommandParser::splitString(const std::string& str, char delimiter)
{
    std::vector<std::string> tokens;
    std::stringstream ss(str);
    std::string token;

    while (std::getline(ss, token, delimiter))
    {
        token = trim(token);
        if (!token.empty())
            tokens.push_back(token);
    }

    return tokens;
}

std::string CommandParser::trim(const std::string& str)
{
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos)
        return "";

    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

// Execute IRC commands
void Commands::executeCommand(Server* server, Client* client, const IRCMessage& msg)
{
    std::string cmd = msg.command;
    std::transform(cmd.begin(), cmd.end(), cmd.begin(), ::toupper);

    std::cout << "Executing command: " << cmd << " for client " << client->getClientFd() << std::endl;

    if (cmd == "PASS")
        handlePASS(server, client, msg);
    else if (cmd == "NICK")
        handleNICK(server, client, msg);
    else if (cmd == "USER")
        handleUSER(server, client, msg);
    else if (cmd == "PING")
        handlePING(server, client, msg);
    else if (cmd == "QUIT")
        handleQUIT(server, client, msg);
    else if (cmd == "CAP")
    {
        handleCAP(client->getClientFd(), msg, *client);
    }
    else
    {
        std::cout << "Unknown command: " << cmd << std::endl;
    }
}

// ...existing code...

void Commands::handleCAP(int client_fd, const IRCMessage& message, Client& client) {
    (void)client; // Unused parameter

    if (message.params.empty()) {
        std::string response = ":localhost 410 * :Invalid CAP command\r\n";
        send(client_fd, response.c_str(), response.length(), 0);
        return;
    }

    std::string subcommand = message.params[0];

    if (subcommand == "LS" || subcommand == "LIST") {
        // Boş yetenek listesi gönder (basit sunucu)
        std::string response = ":localhost CAP * LS :\r\n";
        send(client_fd, response.c_str(), response.length(), 0);
    }
    else if (subcommand == "REQ") {
        // Hiçbir yetenek desteklemediğimizi söyle
        std::string response = ":localhost CAP * NAK :\r\n";
        send(client_fd, response.c_str(), response.length(), 0);
    }
    else if (subcommand == "END") {
        // CAP negotiation'ı sonlandır - hiçbir şey yapma
        // İstemci normal komutları göndermeye devam edecek
    }
}

void Commands::handlePASS(Server* server, Client* client, const IRCMessage& msg)
{
    if (client->isRegistered())
    {
        sendReply(server, client, ERR_ALREADYREGISTRED(client->getNickname()));
        return;
    }

    if (msg.params.empty())
    {
        sendReply(server, client, ERR_NEEDMOREPARAMS("*", "PASS"));
        return;
    }

    std::string password = msg.params[0];
    if (password == server->getPassword())
    {
        client->setPassword(true);
        std::cout << "Client " << client->getClientFd() << " provided correct password" << std::endl;
    }
    else
    {
        sendReply(server, client, ERR_PASSWDMISMATCH("*"));
        std::cout << "Client " << client->getClientFd() << " provided wrong password" << std::endl;
    }
}

void Commands::handleNICK(Server* server, Client* client, const IRCMessage& msg)
{
    if (msg.params.empty())
    {
        sendReply(server, client, ERR_NEEDMOREPARAMS(
    		(client->getNickname().empty() ? std::string("*") : client->getNickname()),
    		std::string("NICK")
		));
        return;
    }

    std::string new_nick = msg.params[0];

    // Check if nickname is already in use
    // TODO: Implement nickname collision detection

    client->setNickname(new_nick);
    std::cout << "Client " << client->getClientFd() << " set nickname to: " << new_nick << std::endl;

    // Send welcome if client is now fully registered
    if (client->isRegistered())
    {
        sendWelcome(server, client);
    }
}

void Commands::handleUSER(Server* server, Client* client, const IRCMessage& msg)
{
    if (client->hasUser())
    {
        sendReply(server, client, ERR_ALREADYREGISTRED(client->getNickname()));
        return;
    }

    if (msg.params.size() < 3 || msg.trailing.empty())
    {
        sendReply(server, client, ERR_NEEDMOREPARAMS(
    		(client->getNickname().empty() ? std::string("*") : client->getNickname()),
    		std::string("USER")
		));
        return;
    }

    std::string username = msg.params[0];
    std::string realname = msg.trailing;

    client->setUsername(username);
    client->setRealname(realname);

    std::cout << "Client " << client->getClientFd() << " set username to: " << username << ", realname: " << realname << std::endl;

    // Send welcome if client is now fully registered
    if (client->isRegistered())
    {
        sendWelcome(server, client);
    }
}

void Commands::handlePING(Server* server, Client* client, const IRCMessage& msg)
{
    if (msg.params.empty())
    {
        sendReply(server, client, ERR_NEEDMOREPARAMS(client->getNickname(), "PING"));
        return;
    }

    std::string pong_reply = ":localhost PONG localhost :" + msg.params[0] + "\r\n";
    sendReply(server, client, pong_reply);
}

void Commands::handleQUIT(Server* server, Client* client, const IRCMessage& msg)
{
    std::string quit_msg = msg.trailing.empty() ? "Client Quit" : msg.trailing;
    std::cout << "Client " << client->getClientFd() << " is quitting: " << quit_msg << std::endl;

    // TODO: Notify other clients in channels
    server->removeClient(client->getClientFd());
}

void Commands::sendWelcome(Server* server, Client* client)
{
    std::string nick = client->getNickname();

    sendReply(server, client, RPL_WELCOME(nick));
    sendReply(server, client, RPL_YOURHOST(nick));
    sendReply(server, client, RPL_CREATED(nick));
    sendReply(server, client, RPL_MYINFO(nick));

    std::cout << "Sent welcome messages to " << nick << std::endl;
}

void Commands::sendReply(Server* server, Client* client, const std::string& reply)
{
    server->sendToClient(client->getClientFd(), reply);
}
