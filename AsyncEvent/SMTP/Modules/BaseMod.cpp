#include "BaseMod.hpp"
#include "../../../Logger/Logger.hpp"
#include "../../../Config/GlobalConf.hpp"
#include "../../../HUtils/HStrings.hpp"
#include "../../../HUtils/HNet.hpp"

namespace smtp {

void BaseMod::RegisterCommands(CommandsMap &cmd_map_)
{
    // Bind commands handlers to names and add in commands map.
    // All results must be true.
    auto ins_res = cmd_map_.emplace("helo", std::bind(&BaseMod::_Helo_Handler, std::placeholders::_1, std::placeholders::_2)).second;
    ins_res &= cmd_map_.emplace("mail", std::bind(&BaseMod::_Mail_Handler, std::placeholders::_1, std::placeholders::_2)).second;
    ins_res &= cmd_map_.emplace("rcpt", std::bind(&BaseMod::_Rcpt_Handler, std::placeholders::_1, std::placeholders::_2)).second;
    ins_res &= cmd_map_.emplace("data", std::bind(&BaseMod::_Data_Handler, std::placeholders::_1, std::placeholders::_2)).second;
    ins_res &= cmd_map_.emplace("vrfy", std::bind(&BaseMod::_Vrfy_Handler, std::placeholders::_1, std::placeholders::_2)).second;
    ins_res &= cmd_map_.emplace("rset", std::bind(&BaseMod::_Rset_Handler, std::placeholders::_1, std::placeholders::_2)).second;
    ins_res &= cmd_map_.emplace("help", std::bind(&BaseMod::_Help_Handler, std::placeholders::_1, std::placeholders::_2)).second;
    ins_res &= cmd_map_.emplace("noop", std::bind(&BaseMod::_Noop_Handler, std::placeholders::_1, std::placeholders::_2)).second;
    ins_res &= cmd_map_.emplace("quit", std::bind(&BaseMod::_Quit_Handler, std::placeholders::_1, std::placeholders::_2)).second;

    // Check for conflicts.
    if (!ins_res) {
        log_debug("Smtp commands conflict!");
        throw std::logic_error("Commands adding conflict!");
    }

}

SmtpErr BaseMod::_Helo_Handler(const std::string &cmd_line_, SmtpState &state_)
{
    // Check line syntax.
    std::string helo_arg;
    SmtpErr rerr = smtp::utils::parse_helo(cmd_line_, helo_arg);

    if (rerr != SmtpErr::noerror) {
        state_.close_conn = true;
        return rerr;
    }

    // Check argument syntax (RFC 5321 4.1.1.1).
    hstrings::trim(helo_arg);
    if (!hnet::is_fqdn(helo_arg) && !hnet::is_ip_v4(helo_arg)) {
        rerr = SmtpErr::syntax;
        state_.close_conn = true;
        return rerr;
    }

    // Init helo status in SMTP state.
    state_.client_hello.text = helo_arg;
    state_.client_hello.inited = true;

    // Set reply.
    state_.curent_reply = "250 " + Config::glob().get_conf().primary_hostname + "\r\n";
    return SmtpErr::noerror;
}

SmtpErr BaseMod::_Mail_Handler(const std::string &cmd_line_, SmtpState &state_)
{
    // Check helo status (must be true).
    if (!state_.client_hello.inited)
        return smtp::SmtpErr::heloneed;

    // Check line and arguments syntax.
    EmailAddr email_tmp;
    SmtpErr rerr = utils::parse_mail_from(cmd_line_, email_tmp);

    if (rerr != SmtpErr::noerror)
        return rerr;

    // Set mailfrom is SMTP state.
    state_.mailform = email_tmp;

    // Set reply.
    state_.curent_reply = "250 " + email_tmp.text + " Parsed email:" + state_.mailform.text + "\r\n";
    return smtp::SmtpErr::noerror;
}

SmtpErr BaseMod::_Rcpt_Handler(const std::string &cmd_line_, SmtpState &state_)
{
    // Check mailfrom status.
    if (!state_.mailform.inited)
        return smtp::SmtpErr::sync;

    // Check line and arguments syntax.
    EmailAddr email_tmp;
    SmtpErr rerr = utils::parse_rcpt_to(cmd_line_, email_tmp);

    if (rerr != SmtpErr::noerror)
        return rerr;

    // Check for duplication.
    auto dupcheck = state_.recipients.list.find(email_tmp);
    if (dupcheck == state_.recipients.list.end()) {
        state_.recipients.list.insert(email_tmp);
        state_.curent_reply = "250 OK. Parsed email:" + email_tmp.text + "\r\n";
    }
    else {
        state_.curent_reply = "253 OK. Parsed email:" + email_tmp.text + " Duplicate. Ignored. \r\n";
    }
    return SmtpErr::noerror;
}

SmtpErr BaseMod::_Data_Handler(const std::string &cmd_line_, SmtpState &state_)
{
    // Check mailfrom status.
    if (state_.recipients.list.empty())
        return smtp::SmtpErr::no_rcpts;


    SmtpErr rerr = utils::parse_data(cmd_line_);
    if (rerr != SmtpErr::noerror)
        return rerr;

    state_.waiting_for_data = true;
    state_.curent_reply = "354 OK. Redy for accept data. \r\n";
    return SmtpErr::noerror;
}

SmtpErr BaseMod::_Vrfy_Handler(const std::string &cmd_line_, SmtpState &state_)
{
    state_.curent_reply = "Command not supported yet.\r\n";
    return SmtpErr::noerror;
}

SmtpErr BaseMod::_Rset_Handler(const std::string &cmd_line_, SmtpState &state_)
{
    state_.curent_reply = "Command not supported yet.\r\n";
    return SmtpErr::noerror;
}

SmtpErr BaseMod::_Help_Handler(const std::string &cmd_line_, SmtpState &state_)
{
    state_.curent_reply = "Command not supported yet.\r\n";
    return SmtpErr::noerror;
}

SmtpErr BaseMod::_Noop_Handler(const std::string &cmd_line_, SmtpState &state_)
{
    state_.curent_reply = "Command not supported yet.\r\n";
    return SmtpErr::noerror;
}

SmtpErr BaseMod::_Quit_Handler(const std::string &cmd_line_, SmtpState &state_)
{
    state_.close_conn = true;
    state_.curent_reply = "221 Bye!\r\n";
    return SmtpErr::noerror;
}


} // namespace smtp
