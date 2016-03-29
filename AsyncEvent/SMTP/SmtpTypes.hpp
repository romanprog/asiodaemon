#ifndef SMTPTYPES_HPP
#define SMTPTYPES_HPP

#include <string>
#include <unordered_map>
namespace smtp {

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
    corrupt
};

struct EmailAddr
{
    std::string text;
};
struct HeloArg
{
    std::string text;
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

struct SmtpUtils
{
    static SmtpCmd cmd_define(const std::string & line);
};

} // namespase

#endif // SMTPTYPES_HPP
