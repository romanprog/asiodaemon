#include "SmtpBuffer.hpp"
#include "../DNS/AEvDnsClient.hpp"
#include <cstring>
#include <iostream>


SmtpCmdBuffer::SmtpCmdBuffer()
    :ParsingBuffAbstract::ParsingBuffAbstract("\r\n", 2048)
{

}

void SmtpCmdBuffer::clear()
{
    reset();
    lines_list = std::queue<std::string>();
    parsed_cmd.clear();
}

std::string SmtpCmdBuffer::get_line()
{
    std::string res;

    if (!lines_list.empty()) {
        res = lines_list.front();
        lines_list.pop();
    }

    return res;
}

bool SmtpCmdBuffer::is_empty()
{
    return lines_list.empty();
}

size_t SmtpCmdBuffer::list_size() const
{
    return lines_list.size();
}

void SmtpCmdBuffer::when_have_new_part(const size_t begin_offset, const size_t size)
{
    lines_list.emplace(data()+begin_offset, size - get_delimiter().size());
    return;
}

// ======================= Data buffer =========================

SmtpDataBuffer::SmtpDataBuffer()
    :ParsingBuffAbstract::ParsingBuffAbstract("\r\n.\r\n", 70000)
{

}

void SmtpDataBuffer::clear()
{
    reset();
    data_end_offset = 0;
}

std::string SmtpDataBuffer::get_data()
{
    std::string res(data(), data_end_offset);
    return res;
}

bool SmtpDataBuffer::is_redy()
{
    return (data_end_offset > 0);
}


void SmtpDataBuffer::when_have_new_part(const size_t begin_offset, const size_t size)
{
    if (is_redy())
        return;

    data_end_offset = size - get_delimiter().size();
}
