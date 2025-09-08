#include "../includes/IRCResponse.hpp"
#include <sstream>

// Error responses
std::string IRCResponse::createErrorNeedMoreParams(const std::string& nick, const std::string& command)
{
    std::ostringstream oss;
    oss << ":server 461 " << nick << " " << command << " :Not enough parameters\r\n";
    return oss.str();
}

std::string IRCResponse::createErrorNoNicknameGiven(const std::string& nick)
{
    std::ostringstream oss;
    oss << ":server 431 " << nick << " :No nickname given\r\n";
    return oss.str();
}

std::string IRCResponse::createErrorErroneusNickname(const std::string& nick, const std::string& badNick)
{
    std::ostringstream oss;
    oss << ":server 432 " << nick << " " << badNick << " :Erroneus nickname\r\n";
    return oss.str();
}

std::string IRCResponse::createErrorNicknameInUse(const std::string& nick, const std::string& usedNick)
{
    std::ostringstream oss;
    oss << ":server 433 " << nick << " " << usedNick << " :Nickname is already in use\r\n";
    return oss.str();
}

std::string IRCResponse::createErrorNotRegistered(const std::string& nick)
{
    std::ostringstream oss;
    oss << ":server 451 " << nick << " :You have not registered\r\n";
    return oss.str();
}

std::string IRCResponse::createErrorAlreadyRegistered(const std::string& nick)
{
    std::ostringstream oss;
    oss << ":server 462 " << nick << " :You may not reregister\r\n";
    return oss.str();
}

std::string IRCResponse::createErrorPasswdMismatch(const std::string& nick)
{
    std::ostringstream oss;
    oss << ":server 464 " << nick << " :Password incorrect\r\n";
    return oss.str();
}

std::string IRCResponse::createErrorNoSuchChannel(const std::string& nick, const std::string& channel)
{
    std::ostringstream oss;
    oss << ":server 403 " << nick << " " << channel << " :No such channel\r\n";
    return oss.str();
}

std::string IRCResponse::createErrorNotOnChannel(const std::string& nick, const std::string& channel)
{
    std::ostringstream oss;
    oss << ":server 442 " << nick << " " << channel << " :You're not on that channel\r\n";
    return oss.str();
}

std::string IRCResponse::createErrorNoSuchNick(const std::string& nick, const std::string& target)
{
    std::ostringstream oss;
    oss << ":server 401 " << nick << " " << target << " :No such nick/channel\r\n";
    return oss.str();
}

std::string IRCResponse::createErrorCannotSendToChan(const std::string& nick, const std::string& channel)
{
    std::ostringstream oss;
    oss << ":server 404 " << nick << " " << channel << " :Cannot send to channel\r\n";
    return oss.str();
}

// Success responses
std::string IRCResponse::createWelcome(const std::string& nick, const std::string& user, const std::string& host)
{
    std::ostringstream oss;
    oss << ":server 001 " << nick << " :Welcome to the Internet Relay Network " << nick << "!" << user << "@" << host << "\r\n";
    return oss.str();
}

std::string IRCResponse::createYourHost(const std::string& nick, const std::string& serverName)
{
    std::ostringstream oss;
    oss << ":server 002 " << nick << " :Your host is " << serverName << ", running version 1.0\r\n";
    return oss.str();
}

std::string IRCResponse::createCreated(const std::string& nick, const std::string& date)
{
    std::ostringstream oss;
    oss << ":server 003 " << nick << " :This server was created " << date << "\r\n";
    return oss.str();
}

std::string IRCResponse::createMyInfo(const std::string& nick, const std::string& serverName)
{
    std::ostringstream oss;
    oss << ":server 004 " << nick << " " << serverName << " 1.0 o o\r\n";
    return oss.str();
}

std::string IRCResponse::createPong(const std::string& serverName, const std::string& token)
{
    std::ostringstream oss;
    oss << ":" << serverName << " PONG " << serverName << " :" << token << "\r\n";
    return oss.str();
}

std::string IRCResponse::createJoin(const std::string& nick, const std::string& user, const std::string& host, const std::string& channel)
{
    std::ostringstream oss;
    oss << ":" << nick << "!" << user << "@" << host << " JOIN " << channel << "\r\n";
    return oss.str();
}

std::string IRCResponse::createPart(const std::string& nick, const std::string& user, const std::string& host, const std::string& channel)
{
    std::ostringstream oss;
    oss << ":" << nick << "!" << user << "@" << host << " PART " << channel << "\r\n";
    return oss.str();
}
