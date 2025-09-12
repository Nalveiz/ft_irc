#ifndef IRCRESPONSE_HPP
#define IRCRESPONSE_HPP

#include <string>
#include <sstream>
#include <sstream>

class IRCResponse
{
public:
    // Error responses
    static std::string createErrorNeedMoreParams(const std::string& nick, const std::string& command);
    static std::string createErrorNoNicknameGiven(const std::string& nick);
    static std::string createErrorErroneusNickname(const std::string& nick, const std::string& badNick);
    static std::string createErrorNicknameInUse(const std::string& nick, const std::string& usedNick);
    static std::string createErrorNotRegistered(const std::string& nick);
    static std::string createErrorAlreadyRegistered(const std::string& nick);
    static std::string createErrorPasswdMismatch(const std::string& nick);
    static std::string createErrorNoSuchChannel(const std::string& nick, const std::string& channel);
    static std::string createErrorNotOnChannel(const std::string& nick, const std::string& channel);
    static std::string createErrorNoSuchNick(const std::string& nick, const std::string& target);
    static std::string createErrorCannotSendToChan(const std::string& nick, const std::string& channel);
    static std::string createErrorChanOPrivsNeeded(const std::string& nick, const std::string& channel);
    static std::string createErrorUserNotInChannel(const std::string& nick, const std::string& target, const std::string& channel);
    static std::string createErrorUserOnChannel(const std::string& nick, const std::string& target, const std::string& channel);
    static std::string createErrorInviteOnlyChannel(const std::string& nick, const std::string& channel);
    static std::string createErrorTopicOPrivsNeeded(const std::string& nick, const std::string& channel);
    static std::string createErrorBadChannelKey(const std::string& nick, const std::string& channel);
    static std::string createErrorUserOnChannel(const std::string& nick, const std::string& channel);
    static std::string createErrorUnknownCommand(const std::string& nick, const std::string& command);
    static std::string createQUIT(const std::string& nick, const std::string& user, const std::string& host, const std::string& reason);

    // Success responses
    static std::string createWelcome(const std::string& nick, const std::string& user, const std::string& host);
    static std::string createYourHost(const std::string& nick, const std::string& serverName);
    static std::string createCreated(const std::string& nick, const std::string& date);
    static std::string createMyInfo(const std::string& nick, const std::string& serverName);
    static std::string createISupport(const std::string& nick);
    static std::string createPong(const std::string& serverName, const std::string& token);
    static std::string createJoin(const std::string& nick, const std::string& user, const std::string& host, const std::string& channel);
    static std::string createPart(const std::string& nick, const std::string& user, const std::string& host, const std::string& channel);
    static std::string createKick(const std::string& nick, const std::string& user, const std::string& host, const std::string& channel, const std::string& target, const std::string& reason);
    static std::string createInvite(const std::string& nick, const std::string& user, const std::string& host, const std::string& target, const std::string& channel);
    static std::string createInviting(const std::string& nick, const std::string& target, const std::string& channel);

    // Channel listing responses
    static std::string createNamReply(const std::string& nick, const std::string& channel, const std::string& names);
    static std::string createEndOfNames(const std::string& nick, const std::string& channel);
    static std::string createPartWithReason(const std::string& nick, const std::string& user, const std::string& host, const std::string& channel, const std::string& reason);

    // TOPIC responses
    static std::string createTopic(const std::string& nick, const std::string& user, const std::string& host, const std::string& channel, const std::string& topic);
    static std::string createTopicReply(const std::string& nick, const std::string& channel, const std::string& topic);
    static std::string createNoTopicReply(const std::string& nick, const std::string& channel);

    // PRIVMSG responses
    static std::string createPrivmsg(const std::string& nick, const std::string& user, const std::string& host, const std::string& target, const std::string& message);

    // NOTICE responses
    static std::string createNotice(const std::string& nick, const std::string& message);

    // NICK change response
    static std::string createNickChange(const std::string& oldNick, const std::string& user, const std::string& host, const std::string& newNick);

    // MODE responses
    static std::string createModeReply(const std::string& nick, const std::string& channel, const std::string& modes);
    static std::string createUnknownModeFlag(const std::string& nick);
    static std::string createModeChange(const std::string& nick, const std::string& user, const std::string& host, const std::string& channel, const std::string& modes);

private:
    IRCResponse(); // Constructor private to prevent instantiation
};

#endif
