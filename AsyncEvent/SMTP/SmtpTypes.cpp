#include "SmtpTypes.hpp"
#include "../DNS/DnsDataTypes.hpp"
#include "../../HUtils/HStrings.hpp"

namespace smtp {
SmtpCmd SmtpUtils::cmd_define(const std::string &line)
{
    // Split and get first word.
    std::string cmd(hstrings::to_lower(hstrings::splitted(line, ' ', true)[0]));

    if (!hstrings::is_alfa_only(cmd))
        return SmtpCmd::corrupt;

    auto res = cmd_map.find(cmd);

    if (res != cmd_map.end())
        return res->second;

    return SmtpCmd::unknown;
}
}
