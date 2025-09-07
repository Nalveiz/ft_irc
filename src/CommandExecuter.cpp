/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CommandExecuter.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: soksak <soksak@42istanbul.com.tr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/07 19:25:45 by soksak            #+#    #+#             */
/*   Updated: 2025/09/07 21:18:27 by soksak           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/CommandExecuter.hpp"


// Execute IRC commands
void CommandExecuter::executeCommand(Server* server, Client* client, const IRCMessage& msg)
{
    std::string cmd = msg.getCommand();
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

void CommandExecuter::handleCAP(int client_fd, const IRCMessage& message, Client& client) {
    (void)client; // Unused parameter

    if (message.getParams().empty()) {
        std::string response = ":localhost 410 * :Invalid CAP command\r\n";
        send(client_fd, response.c_str(), response.length(), 0);
        return;
    }

    std::string subcommand = message.getParams()[0];

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

void CommandExecuter::handlePASS(Server* server, Client* client, const IRCMessage& msg)
{
    if (client->isRegistered())
    {
        writeClientSendBuffer(server, client, ERR_ALREADYREGISTRED(client->getNickname()));
        return;
    }

    if (msg.getParams().empty())
    {
        writeClientSendBuffer(server, client, ERR_NEEDMOREPARAMS("*", "PASS"));
        return;
    }

    std::string password = msg.getParams()[0];
    if (password == server->getPassword())
    {
        client->setPassword(true);
        std::cout << "Client " << client->getClientFd() << " provided correct password" << std::endl;
    }
    else
    {
        writeClientSendBuffer(server, client, ERR_PASSWDMISMATCH("*"));
        std::cout << "Client " << client->getClientFd() << " provided wrong password" << std::endl;
    }
}

void CommandExecuter::handleNICK(Server* server, Client* client, const IRCMessage& msg)
{
    if (msg.getParams().empty())
    {
        writeClientSendBuffer(server, client, ERR_NEEDMOREPARAMS(
    		(client->getNickname().empty() ? std::string("*") : client->getNickname()),
    		std::string("NICK")
		));
        return;
    }

    std::string new_nick = msg.getParams()[0];

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

void CommandExecuter::handleUSER(Server* server, Client* client, const IRCMessage& msg)
{
    if (client->hasUser())
    {
        writeClientSendBuffer(server, client, ERR_ALREADYREGISTRED(client->getNickname()));
        return;
    }

    if (msg.getParams().size() < 3 || msg.getTrailing().empty())
    {
        writeClientSendBuffer(server, client, ERR_NEEDMOREPARAMS(
    		(client->getNickname().empty() ? std::string("*") : client->getNickname()),
    		std::string("USER")
		));
        return;
    }

    std::string username = msg.getParams()[0];
    std::string realname = msg.getTrailing();

    client->setUsername(username);
    client->setRealname(realname);

    std::cout << "Client " << client->getClientFd() << " set username to: " << username << ", realname: " << realname << std::endl;

    // Send welcome if client is now fully registered
    if (client->isRegistered())
    {
        sendWelcome(server, client);
    }
}

void CommandExecuter::handlePING(Server* server, Client* client, const IRCMessage& msg)
{
    if (msg.getParams().empty())
    {
        writeClientSendBuffer(server, client, ERR_NEEDMOREPARAMS(client->getNickname(), "PING"));
        return;
    }

    std::string pong_reply = ":localhost PONG localhost :" + msg.getParams()[0] + "\r\n";
    writeClientSendBuffer(server, client, pong_reply);
}

void CommandExecuter::handleQUIT(Server* server, Client* client, const IRCMessage& msg)
{
    std::string quit_msg = msg.getTrailing().empty() ? "Client Quit" : msg.getTrailing();
    std::cout << "Client " << client->getClientFd() << " is quitting: " << quit_msg << std::endl;

    // TODO: Notify other clients in channels
    server->removeClient(client->getClientFd());
}

void CommandExecuter::sendWelcome(Server* server, Client* client)
{
    std::string nick = client->getNickname();

    writeClientSendBuffer(server, client, RPL_WELCOME(nick));
    writeClientSendBuffer(server, client, RPL_YOURHOST(nick));
    writeClientSendBuffer(server, client, RPL_CREATED(nick));
    writeClientSendBuffer(server, client, RPL_MYINFO(nick));

    std::cout << "Sent welcome messages to " << nick << std::endl;
}

void CommandExecuter::writeClientSendBuffer(Server* server, Client* client, const std::string& reply)
{
    (void)server; // Unused parameter for now
	client->appendToSendBuffer(reply);
}
