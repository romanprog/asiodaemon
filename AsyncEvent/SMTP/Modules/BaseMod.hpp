#ifndef BASEMOD
#define BASEMOD

#include "../SmtpTypes.hpp"

/// ///////////////////////////////////////////////////////////////
/// Base SMTP protocol: RFC822 commands. 4.1.1.  COMMAND SEMANTICS
/// Supported: HELO, MAIL, RCPT, DATA, RSET, VRFY, HELP, NOOP, QUIT
/// Not supported: SEND, SOML, SAML, EXPN, TURN
/// ///////////////////////////////////////////////////////////////

namespace smtp {

class BaseMod
{
public:
    static void RegisterCommands(smtp::CommandsMap & cmd_map_);

private:

    BaseMod() = default;

    static SmtpErr _Helo_Handler(const std::string & cmd_line_, SmtpState & state_);
    static SmtpErr _Mail_Handler(const std::string & cmd_line_, SmtpState & state_);
    static SmtpErr _Rcpt_Handler(const std::string & cmd_line_, SmtpState & state_);
    static SmtpErr _Data_Handler(const std::string & cmd_line_, SmtpState & state_);
    static SmtpErr _Vrfy_Handler(const std::string & cmd_line_, SmtpState & state_);
    static SmtpErr _Rset_Handler(const std::string & cmd_line_, SmtpState & state_);
    static SmtpErr _Help_Handler(const std::string & cmd_line_, SmtpState & state_);
    static SmtpErr _Noop_Handler(const std::string & cmd_line_, SmtpState & state_);
    static SmtpErr _Quit_Handler(const std::string & cmd_line_, SmtpState & state_);
};

} // namespace smtp

#endif // BASEMOD
