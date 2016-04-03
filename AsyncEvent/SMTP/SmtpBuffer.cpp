#include "SmtpBuffer.hpp"
#include "../DNS/AEvDnsClient.hpp"
#include <cstring>
#include <iostream>


SmtpBuffer::SmtpBuffer()
    :ParsingBuffAbstract::ParsingBuffAbstract("\r\n", 2048)
{
    //    smtp_answer = "220 Welcome my son, welcome to the machine. ESMTP experimental server. My Email: <roman.progonnyj@gmail.com>\r\n";
}

void SmtpBuffer::clear()
{
    reset();
    lines_list = std::queue<std::string>();
    parsed_cmd.clear();
}

std::string SmtpBuffer::get_line()
{
    std::string res;

    if (!lines_list.empty()) {
        res = lines_list.front();
        lines_list.pop();
    }

    return res;
}

bool SmtpBuffer::is_empty()
{
    return lines_list.empty();
}

size_t SmtpBuffer::list_size() const
{
    return lines_list.size();
}

void SmtpBuffer::when_have_new_part(const size_t begin_offset, const size_t size)
{
    lines_list.emplace(data()+begin_offset, size - get_separator().size());
    return;
}

