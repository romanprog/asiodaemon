#include "SmtpTypes.hpp"
#include "../DNS/DnsTypes.hpp"
#include "../DNS/DnsUtils.hpp"
#include "../../HUtils/HStrings.hpp"

namespace smtp {

namespace utils {

namespace {
struct EnumClassHash
{
    template <typename T>
    std::size_t operator()(const T t) const
    {
        return static_cast<std::size_t>(t);
    }
};

const std::unordered_map<SmtpErr, std::string, EnumClassHash> err_mess_map {
    {SmtpErr::heloneed, "503 EHLO/HELO needed."},
    {SmtpErr::syntax, "501 Syntax error in params."},
    {SmtpErr::unrecognized, "500 Syntax error, command unrecognised."},
    {SmtpErr::sync, "503 Bad sequence of commands."},
    {SmtpErr::trcpt, "503 More then 1 rcpts in transaction."},
    {SmtpErr::emlformat, "511 Bad <reverse-path> email addres."},
    {SmtpErr::no_rcpts, "RCPT first."}
};
}

SmtpCmd parse_line(const std::string &line)
{
    if (line.empty())
        return SmtpCmd::empty;

    if (line.size() < 4)
        return SmtpCmd::unknown;

    auto cmd_type = cmd_map.find(get_cmd_str(line));

    if (cmd_type == cmd_map.end())
        return SmtpCmd::unknown;

    return cmd_type->second;
}

SmtpErr parse_helo(const std::string &helo_line, std::string &result)
{
    std::vector<std::string> s_helo;
    hstrings::split(helo_line, s_helo, ' ', true);
    if (s_helo.size() != 2)
        return SmtpErr::syntax;

    result = s_helo[1];
    return SmtpErr::noerror;
}

SmtpErr parse_mail_from(const std::string &mf_line, EmailAddr &result)
{
    std::string str_tmp;

    // "MAIL FROM" part
    hstrings::get_part(mf_line, str_tmp, ':', 0);

    hstrings::to_lower(str_tmp);

    if (str_tmp != "mail from")
        return SmtpErr::unrecognized;

    // Param part.
    hstrings::cut_part(mf_line, str_tmp, ':', 0);
    hstrings::trim(str_tmp);

    // E-mail addres must have syntax '<localpart@domain>'.
    if (*str_tmp.begin() != '<' || *(str_tmp.end()-1) != '>')
        return SmtpErr::syntax;

    std::string eml_str(str_tmp.begin()+1, str_tmp.end() - 1);


    if (!result.set(eml_str))
        return SmtpErr::emlformat;

    return SmtpErr::noerror;

}

std::string err_to_str(SmtpErr err)
{
    auto err_iter = err_mess_map.find(err);
    if (err_iter == err_mess_map.end())
        return std::string();

    return err_iter->second + "\r\n";
}

SmtpErr parse_rcpt_to(const std::string &mf_line, EmailAddr &result)
{
    std::string str_tmp;

    // "RCPT TO" part
    hstrings::get_part(mf_line, str_tmp, ':', 0);

    hstrings::to_lower(str_tmp);

    if (str_tmp != "rcpt to")
        return SmtpErr::unrecognized;

    // Param part.
    hstrings::cut_part(mf_line, str_tmp, ':', 0);
    hstrings::trim(str_tmp);

    // E-mail addres must have syntax '<localpart@domain>'.
    if (*str_tmp.begin() != '<' || *(str_tmp.end()-1) != '>')
        return SmtpErr::syntax;

    std::string forward_path(str_tmp.begin()+1, str_tmp.end() - 1);

    std::string eml_addr;
    hstrings::get_part_reverse(forward_path, eml_addr, ':', 0);

    if (!result.set(eml_addr))
        return SmtpErr::emlformat;

    return SmtpErr::noerror;
}

SmtpErr parse_data(const std::string &mf_line)
{
    std::string str_tmp (mf_line);
    hstrings::to_lower(hstrings::trim(str_tmp));
    if (str_tmp != "data")
        return SmtpErr::unrecognized;

    return SmtpErr::noerror;
}

std::string get_cmd_str(const std::string &line)
{
    std::string cmd;

    if (line.size() >= 4) {
        // First 4 chars !must! contain a command.
        cmd = line.substr(0, 4);
        // Commands and replies are not case sensitive. RFC 821, 2. THE SMTP MODEL
        hstrings::to_lower(cmd);
    }
    return cmd;
}

} // namespace utils

bool EmailAddr::set(std::string email)
{
    hstrings::get_part(email, localpart, '@', 0);
    if ((localpart.size() < min_localpatr_length) || ((localpart.size() > max_localpatr_length)))
        return false;

    hstrings::get_part(email, domain, '@', 1);
    if (!dns::utils::is_fqdn(domain))
        return false;

    text = email;

    inited = true;
    return true;
}

bool operator ==(const EmailAddr &one, const EmailAddr &other)
{
    return one.text == other.text;
}

// namespace utils
} // namespace smtp
