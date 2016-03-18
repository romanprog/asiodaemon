#include "AEvBuffer.hpp"
#include <cstring>
#include <iostream>
namespace aev {

AEvBuffer::AEvBuffer()
    :ParsingBuffAbstract::ParsingBuffAbstract("\r\n", 2048)
{
    smtp_answer = "220 " + primary_domain + " ESMTP experimental server. Email: <roman.progonnyj@gmail.com>" + "\r\n";
}

const std::string &AEvBuffer::answer()
{
    have_answer = false;
    waiting_for_command = true;
    return smtp_answer;
}

const std::string AEvBuffer::get_answer()
{
    return smtp_answer;
}

void AEvBuffer::when_have_new_part(const size_t begin_offset, const size_t size)
{
    std::string answ(data()+begin_offset, size - get_separator().size());
    if (answ == "quit") {
        waiting_for_command = false;
        abort = true;
        have_answer = false;
        return;
    }
    smtp_answer = "Unknown command: " + answ + "\r\n";
    have_answer = true;
    waiting_for_command = false;

}

void AEvBuffer::when_parsed(unsigned new_parts_count)
{

}

}
