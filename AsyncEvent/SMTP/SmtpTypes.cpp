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
    {SmtpErr::emlformat, "511 Bad <reverse-path> email addres."}
};
}

SmtpCmd parse_line(const std::string &line)
{
    if (line.empty())
        return SmtpCmd::empty;

    if (line.size() < 4)
        return SmtpCmd::unknown;

    // First 4 chars !must! contain a command.
    std::string cmd(line.substr(0, 4));

    // Commands and replies are not case sensitive. RFC 821, 2. THE SMTP MODEL
    hstrings::to_lower(cmd);

    auto cmd_type = cmd_map.find(cmd);

    if (cmd_type == cmd_map.end())
        return SmtpCmd::unknown;

    return cmd_type->second;
}

SmtpErr parse_helo(const std::string &helo_line, std::string &result)
{
    std::vector<std::string> s_helo;
    hstrings::split(helo_line, s_helo, ' ', true);
    if (s_helo.size() > 2)
        return SmtpErr::syntax;

    if (s_helo.size() == 2)
        result = s_helo[1];

    return SmtpErr::noerror;
}

SmtpErr parse_mail_from(const std::string &mf_line, EmailAddr &result)
{
    std::string str_tmp;

    // "MAIL FROM" part
    hstrings::get_part(mf_line, str_tmp, ':', 0);

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



}

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

// namespace utils
} // namespace smtp
