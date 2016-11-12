#ifndef SMTPTYPES_HPP
#define SMTPTYPES_HPP

#include <string>
#include <unordered_map>
#include <map>
#include <vector>
#include <unordered_set>
#include <memory>
#include <functional>

namespace smtp {

const size_t min_localpatr_length = 1;
const size_t max_localpatr_length = 128;

struct EmailAddrHasher
{
    template <typename T>
    std::size_t operator()(T&& t) const
    {
        std::hash<std::string> hasher;
        return hasher(t.text);
    }
};

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
    emlformat,
    // Rctp first.
    no_rcpts
};


struct EmailAddr
{
    std::string text;
    std::string localpart;
    std::string domain;

    bool inited {false};

    bool set(std::string email);
    friend bool operator == (const EmailAddr& one, const EmailAddr & other);

    void reset();
};

bool operator == (const EmailAddr& one, const EmailAddr & other);

struct HeloArg
{
    bool inited {false};
    std::string text;
    bool is_fqdn {false};
    std::string ip;
    void reset();
};

using RcptList = std::unordered_set<EmailAddr, EmailAddrHasher>;

struct EmailRcpts
{
    // Valid rcpts count.
    int valid_count {0};
    int failed_count {0};
    // Any "rcpt to" client command must complated in the server side
    // only after server send reply for it. Any others smtp commands from client
    // before all "rcpt to" replies sended is protocol violation, see RFC 2821.
    unsigned int unsended_responds {0};
    RcptList list;
    void reset();
};

struct SmtpState
{
    smtp::EmailRcpts recipients;
    smtp::EmailAddr mailform;
    smtp::HeloArg client_hello;
    std::string client_ip;
    std::string client_ip_ptr;
    bool waiting_for_data{false};
    // Reply on "RCPT TO" command not sended. --- ???
    size_t rcpts_uninited{0};

    std::string curent_reply;
    bool close_conn {false};

    void reset_transaction()
    {
        recipients.reset();
        mailform.reset();
        waiting_for_data = false;
        rcpts_uninited = 0;
        curent_reply.clear();
    }
};

using SmtpStatePtr = std::shared_ptr<SmtpState>;

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

using ConfirmHendler = std::function<void (bool err)>;

using ReplySendedConfirmHandler = std::function<void (SmtpState & proto_state)>;

using CommandHandler = std::function<ReplySendedConfirmHandler (const std::string & cmd, SmtpState & proto_state, SmtpErr & err_)>;

using CommandsMap = std::unordered_map<std::string, CommandHandler>;
using CommandsMapPtr = std::shared_ptr<CommandsMap>;

namespace utils
{
    std::string get_cmd_str(const std::string & line);
    SmtpCmd parse_line(const std::string & line);
    SmtpErr parse_helo(const std::string & helo_line, std::string & result);
    SmtpErr parse_mail_from(const std::string & mf_line, EmailAddr &result);
    // Parse one recipient.
    SmtpErr parse_rcpt_to(const std::string & mf_line, EmailAddr &result);
    SmtpErr parse_data(const std::string & mf_line);
    std::string err_to_str(SmtpErr err);

} // namespace utils

} // namespace smtp

#endif // SMTPTYPES_HPP
