#include "../includes/ModeHandler.hpp"
#include "../includes/Server.hpp"
#include "../includes/Client.hpp"
#include "../includes/Channel.hpp"
#include "../includes/CommandExecuter.hpp"

void ModeHandler::handleMODE(Server* server, Client* client, const IRCMessage& msg)
{
    if (!CommandExecuter::validateBasicCommand(server, client, msg, "MODE"))
        return;

    std::string target = msg.getParams()[0];

    // Check if target is a channel (starts with #)
    if (target[0] == '#')
    {
        Channel* channel = server->getChannel(target);
        if (!channel)
        {
            client->writeAndEnablePollOut(server, IRCResponse::createErrorNoSuchChannel(client->getNickname(), target));
            return;
        }

        // Check if user is in channel
        if (!channel->isUserInChannel(client->getClientFd()))
        {
            client->writeAndEnablePollOut(server, IRCResponse::createErrorNotOnChannel(client->getNickname(), target));
            return;
        }

        // If no mode string provided, show current modes
        if (msg.getParams().size() == 1)
        {
            std::string currentModes = getCurrentModes(channel);
            std::string modeReply = ":server 324 " + client->getNickname() + " " + target + " " + currentModes + "\r\n";
            client->writeAndEnablePollOut(server, modeReply);
            return;
        }

        // Parse mode changes
        std::string modeString = msg.getParams()[1];
        std::vector<std::string> params;
        for (size_t i = 2; i < msg.getParams().size(); ++i)
        {
            params.push_back(msg.getParams()[i]);
        }

        handleChannelMode(server, client, target, modeString, params);
    }
    else
    {
        // User modes not implemented yet
        client->writeAndEnablePollOut(server, ":server 501 " + client->getNickname() + " :Unknown MODE flag\r\n");
    }
}

void ModeHandler::handleChannelMode(Server* server, Client* client, const std::string& channelName, const std::string& modeString, const std::vector<std::string>& params)
{
    Channel* channel = server->getChannel(channelName);
    if (!channel)
        return;

    // Check if user is operator (required for most mode changes)
    if (!channel->isOperator(client->getClientFd()))
    {
        client->writeAndEnablePollOut(server, IRCResponse::createErrorChanOPrivsNeeded(client->getNickname(), channelName));
        return;
    }

    bool adding = true;
    size_t paramIndex = 0;
    std::string appliedModes;
    std::string modeParams;

    for (size_t i = 0; i < modeString.length(); ++i)
    {
        char c = modeString[i];

        if (c == '+') {
            adding = true;
            continue;
        } else if (c == '-') {
            adding = false;
            continue;
        }

        if (!isValidModeChar(c))
            continue;

        bool changed = false;
        switch (c)
        {
            case 'i': // Invite-only
                if (channel->isInviteOnly() != adding) {
                    channel->setInviteOnly(adding);
                    changed = true;
                }
                break;

            case 't': // Topic restricted
                if (channel->isTopicRestricted() != adding) {
                    channel->setTopicRestricted(adding);
                    changed = true;
                }
                break;

            case 'k': // Channel key
                if (adding && paramIndex < params.size()) {
                    channel->setKey(params[paramIndex]);
                    if (!modeParams.empty()) modeParams += " ";
                    modeParams += params[paramIndex];
                    paramIndex++;
                    changed = true;
                } else if (!adding && !channel->getKey().empty()) {
                    channel->setKey("");
                    changed = true;
                }
                break;

            case 'l': // User limit
                if (adding && paramIndex < params.size()) {
                    int limit = atoi(params[paramIndex].c_str());
                    if (limit > 0) {
                        channel->setUserLimit(limit);
                        if (!modeParams.empty()) modeParams += " ";
                        modeParams += params[paramIndex];
                        paramIndex++;
                        changed = true;
                    }
                } else if (!adding && channel->getUserLimit() > 0) {
                    channel->setUserLimit(0);
                    changed = true;
                }
                break;

            case 'o': // Operator privilege
                if (paramIndex < params.size()) {
                    Client* targetClient = server->getClientByNickname(params[paramIndex]);
                    if (targetClient && channel->isUserInChannel(targetClient->getClientFd())) {
                        if (adding) {
                            channel->addOperator(targetClient);
                        } else {
                            channel->removeOperator(targetClient->getClientFd());
                        }
                        if (!modeParams.empty()) modeParams += " ";
                        modeParams += params[paramIndex];
                        changed = true;
                    }
                    paramIndex++;
                }
                break;
        }

        if (changed) {
            appliedModes += (adding ? "+" : "-");
            appliedModes += c;
        }
    }

    // Broadcast mode change to all users in channel
    if (!appliedModes.empty())
    {
        std::string modeMsg = ":" + client->getNickname() + "!" + client->getUsername() + "@localhost MODE " + channelName + " " + appliedModes;
        if (!modeParams.empty())
            modeMsg += " " + modeParams;
        modeMsg += "\r\n";

        channel->broadcast(modeMsg, server, -1);
    }
}

std::string ModeHandler::getCurrentModes(Channel* channel)
{
    std::string modes = "+";

    if (channel->isInviteOnly())
        modes += "i";
    if (channel->isTopicRestricted())
        modes += "t";
    if (!channel->getKey().empty())
        modes += "k";
    if (channel->getUserLimit() > 0)
        modes += "l";

    return modes;
}

bool ModeHandler::isValidModeChar(char mode)
{
    return mode == 'i' || mode == 't' || mode == 'k' || mode == 'o' || mode == 'l';
}
