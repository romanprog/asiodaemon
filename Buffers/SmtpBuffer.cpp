#include "SmtpBuffer.hpp"
#include "../AsyncEvent/DNS/AEvDnsClient.hpp"
#include <cstring>
#include <iostream>


SmtpBuffer::SmtpBuffer()
    :ParsingBuffAbstract::ParsingBuffAbstract("\r\n", 2048)
{
    smtp_answer = "220 Welcome my son, welcome to the machine. ESMTP experimental server. My Email: <roman.progonnyj@gmail.com>\r\n";
}

void SmtpBuffer::parse_smtp(size_t size, SmtpBuffer::SmtpCmdCallback cb)
{
    _when_have_ansver_cb = cb;
    accept(size);
}

const std::string &SmtpBuffer::answer()
{
    have_answer = false;
    waiting_for_command = true;
    return smtp_answer;
}

const std::string SmtpBuffer::get_answer()
{
    return smtp_answer;
}

const std::string SmtpBuffer::get_last_cmd()
{
    return parsed_cmd;
}

void SmtpBuffer::when_have_new_part(const size_t begin_offset, const size_t size)
{
    std::string answ(data()+begin_offset, size - get_separator().size());
    parsed_cmd = answ;
    if (answ == "quit") {
        waiting_for_command = false;
        abort = true;
        have_answer = false;
        _when_have_ansver_cb();
        return;
    }

    _create_child<aev::AEvDnsClient>(3, answ, dns::DnsQType::A,
                                     [this](int err, dns::DnsRespond result)
    {
        smtp_answer.clear();

        if (err) {
            smtp_answer += "Can't resolve.\r\n";
        } else {
            for (auto & r : result.get_answers_list())
                smtp_answer += r.answer + "\r\n";
        }
        have_answer = true;
        waiting_for_command = false;
        _when_have_ansver_cb();

    }
    );


}

void SmtpBuffer::when_parsed(unsigned new_parts_count)
{

}

