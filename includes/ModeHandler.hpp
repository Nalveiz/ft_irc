#ifndef MODEHANDLER_HPP
#define MODEHANDLER_HPP

#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include "IRCMessage.hpp"
#include "IRCResponse.hpp"

// Forward declarations
class Server;
class Client;
class Channel;
class ModeHandler
{
public:
    // Main MODE command handler
    static void handleMODE(Server* server, Client* client, const IRCMessage& msg);

private:
    // Channel mode handlers
    static void handleChannelMode(Server* server, Client* client, const std::string& channel, const std::string& modeString, const std::vector<std::string>& params);

    // Helper functions
    static std::string getCurrentModes(Channel* channel);
    static bool isValidModeChar(char mode);

    // Private constructor - utility class only
    ModeHandler();
    ModeHandler(const ModeHandler& other);
    ModeHandler& operator=(const ModeHandler& other);
};

#endif
