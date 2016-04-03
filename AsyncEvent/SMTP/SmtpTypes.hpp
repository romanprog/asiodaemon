#ifndef SMTPTYPES_HPP
#define SMTPTYPES_HPP

#include <string>
#include <unordered_map>
#include <map>
namespace smtp {

const size_t min_localpatr_length = 1;
const size_t max_localpatr_length = 128;


enum class SmtpCmd
{
    helo,
    mail,
    rcpt,
    data,
    rset,
    vrfy,
    expn,
    help,
    noop,
    quit,
    unknown,
    empty
};

enum class SmtpErr
{
    noerror = 0,
    // Need helo/ehlo first.
    heloneed,
    // More then 1 rcpt per transaction.
    trcpt,
    // Synchronization error.
    sync,
    // Unacceptable symbols.
    corrupt,
    // Unrecognized command.
    unrecognized,
    // Syntax error.
    syntax,
    // Email addres format invalid.
    emlformat
};

struct EmailAddr
{
    std::string text;
    std::string localpart;
    std::string domain;

    bool inited;
    bool set(std::string email);
};
struct HeloArg
{
    bool inited {false};
    std::string text;
    bool is_fqdn {false};
    std::string ip;
};

const std::unordered_map<std::string, SmtpCmd> cmd_map {
    {"helo", SmtpCmd::helo},
    {"mail", SmtpCmd::mail},
    {"rcpt", SmtpCmd::rcpt},
    {"data", SmtpCmd::data},
    {"vrfy", SmtpCmd::vrfy},
    {"expn", SmtpCmd::expn},
    {"helo", SmtpCmd::helo},
    {"help", SmtpCmd::help},
    {"noop", SmtpCmd::noop},
    {"quit", SmtpCmd::quit}
};

namespace utils
{
    SmtpCmd parse_line(const std::string & line);
    SmtpErr parse_helo(const std::string & helo_line, std::string & result);
    SmtpErr parse_mail_from(const std::string & mf_line, EmailAddr &result);
    std::string err_to_str(SmtpErr err);

} // namespace utils

} // namespace smtp

#endif // SMTPTYPES_HPP
