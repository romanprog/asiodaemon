#include "AEvBuffer.hpp"
#include <cstring>
#include <iostream>
namespace aev {

AEvBuffer::AEvBuffer()
    :ParsingBuffAbstract::ParsingBuffAbstract("\r\n", 2048)
{

}

void AEvBuffer::when_have_new_part(const size_t begin_offset, const size_t size)
{

}

void AEvBuffer::when_parsed(unsigned new_parts_count)
{

}

}
