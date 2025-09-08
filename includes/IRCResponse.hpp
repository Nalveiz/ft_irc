#ifndef IRCRESPONSE_HPP
#define IRCRESPONSE_HPP

#include <string>

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

    // Success responses
    static std::string createWelcome(const std::string& nick, const std::string& user, const std::string& host);
    static std::string createYourHost(const std::string& nick, const std::string& serverName);
    static std::string createCreated(const std::string& nick, const std::string& date);
    static std::string createMyInfo(const std::string& nick, const std::string& serverName);
    static std::string createPong(const std::string& serverName, const std::string& token);
    static std::string createJoin(const std::string& nick, const std::string& user, const std::string& host, const std::string& channel);
    static std::string createPart(const std::string& nick, const std::string& user, const std::string& host, const std::string& channel);

private:
    IRCResponse(); // Constructor private to prevent instantiation
};

#endif
