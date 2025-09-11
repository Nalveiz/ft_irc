#include "../includes/IRCResponse.hpp"

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

std::string IRCResponse::createErrorChanOPrivsNeeded(const std::string& nick, const std::string& channel)
{
    std::ostringstream oss;
    oss << ":server 482 " << nick << " " << channel << " :You're not channel operator\r\n";
    return oss.str();
}

std::string IRCResponse::createErrorUserNotInChannel(const std::string& nick, const std::string& target, const std::string& channel)
{
    std::ostringstream oss;
    oss << ":server 441 " << nick << " " << target << " " << channel << " :They aren't on that channel\r\n";
    return oss.str();
}

std::string IRCResponse::createErrorUserOnChannel(const std::string& nick, const std::string& target, const std::string& channel)
{
    std::ostringstream oss;
    oss << ":server 443 " << nick << " " << target << " " << channel << " :is already on channel\r\n";
    return oss.str();
}

std::string IRCResponse::createErrorInviteOnlyChannel(const std::string& nick, const std::string& channel)
{
    std::ostringstream oss;
    oss << ":server 473 " << nick << " " << channel << " :This channel is invite only\r\n";
    return oss.str();
}

std::string IRCResponse::createErrorTopicOPrivsNeeded(const std::string& nick, const std::string& channel)
{
    std::ostringstream oss;
    oss << ":server 482 " << nick << " " << channel << " :You're not channel operator\r\n";
    return oss.str();
}

std::string IRCResponse::createErrorBadChannelKey(const std::string& nick, const std::string& channel)
{
    std::ostringstream oss;
    oss << ":server 475 " << nick << " " << channel << " :Cannot join channel (+k)\r\n";
    return oss.str();
}

std::string IRCResponse::createErrorUserOnChannel(const std::string& nick, const std::string& channel)
{
    std::ostringstream oss;
    oss << ":server 443 " << nick << " " << channel << " :is already on channel\r\n";
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
    // Format: 004 <nick> <servername> <version> <available user modes> <available channel modes>
    // Channel modes: i(invite-only), t(topic-restricted), k(key), l(limit), o(operator)
    oss << ":server 004 " << nick << " " << serverName << " 1.0 o itklo\r\n";
    return oss.str();
}

std::string IRCResponse::createISupport(const std::string& nick)
{
    std::ostringstream oss;
    // 005 ISUPPORT - Advertise server features
    // CHANMODES: Channel modes organized by type
    // PREFIX: User prefixes (operator @)
    // CHANTYPES: Supported channel types (#)
    oss << ":server 005 " << nick << " CHANMODES=,,,itkl PREFIX=(o)@ CHANTYPES=# :are supported by this server\r\n";
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

std::string IRCResponse::createKick(const std::string& nick, const std::string& user, const std::string& host, const std::string& channel, const std::string& target, const std::string& reason)
{
    std::ostringstream oss;
    oss << ":" << nick << "!" << user << "@" << host << " KICK " << channel << " " << target << " :" << reason << "\r\n";
    return oss.str();
}

std::string IRCResponse::createNamReply(const std::string& nick, const std::string& channel, const std::string& names)
{
    std::ostringstream oss;
    oss << ":server 353 " << nick << " = " << channel << " :" << names << "\r\n";
    return oss.str();
}

std::string IRCResponse::createEndOfNames(const std::string& nick, const std::string& channel)
{
    std::ostringstream oss;
    oss << ":server 366 " << nick << " " << channel << " :End of /NAMES list\r\n";
    return oss.str();
}

std::string IRCResponse::createPartWithReason(const std::string& nick, const std::string& user, const std::string& host, const std::string& channel, const std::string& reason)
{
    std::ostringstream oss;
    oss << ":" << nick << "!" << user << "@" << host << " PART " << channel << " :" << reason << "\r\n";
    return oss.str();
}

std::string IRCResponse::createInvite(const std::string& nick, const std::string& user, const std::string& host, const std::string& target, const std::string& channel)
{
    std::ostringstream oss;
    oss << ":" << nick << "!" << user << "@" << host << " INVITE " << target << " " << channel << "\r\n";
    return oss.str();
}

std::string IRCResponse::createInviting(const std::string& nick, const std::string& target, const std::string& channel)
{
    std::ostringstream oss;
    oss << ":server 341 " << nick << " " << target << " " << channel << "\r\n";
    return oss.str();
}

std::string IRCResponse::createTopic(const std::string& nick, const std::string& user, const std::string& host, const std::string& channel, const std::string& topic)
{
    std::ostringstream oss;
    oss << ":" << nick << "!" << user << "@" << host << " TOPIC " << channel << " :" << topic << "\r\n";
    return oss.str();
}

std::string IRCResponse::createTopicReply(const std::string& nick, const std::string& channel, const std::string& topic)
{
    std::ostringstream oss;
    oss << ":server 332 " << nick << " " << channel << " :" << topic << "\r\n";
    return oss.str();
}

std::string IRCResponse::createNoTopicReply(const std::string& nick, const std::string& channel)
{
    std::ostringstream oss;
    oss << ":server 331 " << nick << " " << channel << " :No topic is set\r\n";
    return oss.str();
}

// NOTICE responses
std::string IRCResponse::createNotice(const std::string& nick, const std::string& message)
{
    std::ostringstream oss;
    oss << ":server NOTICE " << nick << " :" << message << "\r\n";
    return oss.str();
}

// NICK change response
std::string IRCResponse::createNickChange(const std::string& oldNick, const std::string& user, const std::string& host, const std::string& newNick)
{
    std::ostringstream oss;
    oss << ":" << oldNick << "!" << user << "@" << host << " NICK " << newNick << "\r\n";
    return oss.str();
}

// PRIVMSG response
std::string IRCResponse::createPrivmsg(const std::string& nick, const std::string& user, const std::string& host, const std::string& target, const std::string& message)
{
    std::ostringstream oss;
    oss << ":" << nick << "!" << user << "@" << host << " PRIVMSG " << target << " :" << message << "\r\n";
    return oss.str();
}

// MODE responses
std::string IRCResponse::createModeReply(const std::string& nick, const std::string& channel, const std::string& modes)
{
    std::ostringstream oss;
    oss << ":server 324 " << nick << " " << channel << " " << modes << "\r\n";
    return oss.str();
}

std::string IRCResponse::createUnknownModeFlag(const std::string& nick)
{
    std::ostringstream oss;
    oss << ":server 501 " << nick << " :Unknown MODE flag\r\n";
    return oss.str();
}

std::string IRCResponse::createModeChange(const std::string& nick, const std::string& user, const std::string& host, const std::string& channel, const std::string& modes)
{
    std::ostringstream oss;
    oss << ":" << nick << "!" << user << "@" << host << " MODE " << channel << " " << modes << "\r\n";
    return oss.str();
}
