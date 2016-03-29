#include "SmtpBuffer.hpp"
#include "../DNS/AEvDnsClient.hpp"
#include <cstring>
#include <iostream>


SmtpBuffer::SmtpBuffer()
    :ParsingBuffAbstract::ParsingBuffAbstract("\r\n", 2048)
{
//    smtp_answer = "220 Welcome my son, welcome to the machine. ESMTP experimental server. My Email: <roman.progonnyj@gmail.com>\r\n";
}

std::string SmtpBuffer::get_line()
{
    std::string res;

    if (!lines_list.empty()) {
        res = lines_list.front();
        lines_list.pop();
    }

    if (lines_list.empty())
        have_line = false;

    return res;
}

bool SmtpBuffer::have_new_line()
{
    return have_line;
}

void SmtpBuffer::when_have_new_part(const size_t begin_offset, const size_t size)
{
    lines_list.emplace(data()+begin_offset, size - get_separator().size());
    have_line = true;
    return;




}

void SmtpBuffer::when_parsed(unsigned new_parts_count)
{

}

