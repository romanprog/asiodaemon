#ifndef BASEMOD
#define BASEMOD

#include "../SmtpTypes.hpp"
#include "../IncSmtpState.hpp"

/// ///////////////////////////////////////////////////////////////
/// Base SMTP protocol: RFC822 commands. 4.1.1.  COMMAND SEMANTICS
/// Supported: HELO, MAIL, RCPT, DATA, RSET, VRFY, HELP, NOOP, QUIT
/// Not supported: SEND, SOML, SAML, EXPN, TURN
/// ///////////////////////////////////////////////////////////////

namespace smtp {

using ReplySendedConfirmHandler = std::function<void (smtp::IncSmtpState & proto_state)>;
using CommandHandler = std::function<ReplySendedConfirmHandler (const std::string & cmd, IncSmtpState & proto_state, SmtpErr & err_)>;
using CommandsMap = std::unordered_map<std::string, CommandHandler>;

class BaseMod
{
public:
    static void RegisterCommands(smtp::CommandsMap & cmd_map_);

private:

    BaseMod() = default;

    static ReplySendedConfirmHandler _Helo_Handler(const std::string & cmd_line_, IncSmtpState & state_, SmtpErr & err_);
    static ReplySendedConfirmHandler _Mail_Handler(const std::string & cmd_line_, IncSmtpState & state_, SmtpErr & err_);
    static ReplySendedConfirmHandler _Rcpt_Handler(const std::string & cmd_line_, IncSmtpState & state_, SmtpErr & err_);
    static ReplySendedConfirmHandler _Data_Handler(const std::string & cmd_line_, IncSmtpState & state_, SmtpErr & err_);
    static ReplySendedConfirmHandler _Vrfy_Handler(const std::string & cmd_line_, IncSmtpState & state_, SmtpErr & err_);
    static ReplySendedConfirmHandler _Rset_Handler(const std::string & cmd_line_, IncSmtpState & state_, SmtpErr & err_);
    static ReplySendedConfirmHandler _Help_Handler(const std::string & cmd_line_, IncSmtpState & state_, SmtpErr & err_);
    static ReplySendedConfirmHandler _Noop_Handler(const std::string & cmd_line_, IncSmtpState & state_, SmtpErr & err_);
    static ReplySendedConfirmHandler _Quit_Handler(const std::string & cmd_line_, IncSmtpState & state_, SmtpErr & err_);
};

} // namespace smtp

#endif // BASEMOD
