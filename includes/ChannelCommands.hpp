/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ChannelCommands.hpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: soksak <soksak@42istanbul.com.tr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/11 00:00:00 by soksak            #+#    #+#             */
/*   Updated: 2025/09/11 20:44:44 by soksak           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CHANNELCOMMANDS_HPP
#define CHANNELCOMMANDS_HPP

#include "IRCMessage.hpp"
#include "IRCResponse.hpp"
#include <string>
#include <vector>
#include <algorithm>
#include <iostream>

// Forward declarations to avoid circular dependencies
class Server;
class Client;
class Channel;

class ChannelCommands
{
    public:
        // Channel management commands
        static void handleJOIN(Server* server, Client* client, const IRCMessage& msg);
        static void handlePART(Server* server, Client* client, const IRCMessage& msg);
        static void handleKICK(Server* server, Client* client, const IRCMessage& msg);
        static void handleINVITE(Server* server, Client* client, const IRCMessage& msg);
        static void handleTOPIC(Server* server, Client* client, const IRCMessage& msg);
        static void handleMODE(Server* server, Client* client, const IRCMessage& msg);

        // Helper function for basic command validation
        static bool validateBasicCommand(Server* server, Client* client, const IRCMessage& msg, const std::string& commandName);

    private:
        // Private constructor - this is a utility class with static methods only
        ChannelCommands();
        ChannelCommands(const ChannelCommands& other);
        ChannelCommands& operator=(const ChannelCommands& other);
};

#endif // CHANNELCOMMANDS_HPP
