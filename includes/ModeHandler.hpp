#ifndef MODEHANDLER_HPP
#define MODEHANDLER_HPP

#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include "IRCMessage.hpp"
#include "IRCResponse.hpp"

class Server;
class Client;
class Channel;

class ModeHandler
{
public:
	static void handleMODE(Server *server, Client *client, const IRCMessage &msg);
	static std::string getFullModeString(Channel *channel);

private:
	static void handleChannelMode(Server *server, Client *client, std::string &channel, std::string &modeString, std::vector<std::string> &params);
	static std::string getCurrentModes(Channel *channel);
	static bool isValidModeChar(char mode);

	ModeHandler();
	ModeHandler(const ModeHandler &other);
	ModeHandler &operator=(const ModeHandler &other);
};

#endif
