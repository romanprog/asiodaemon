#include "RESPBuffer.hpp"

RESPBuffer::RESPBuffer()
{

}

void RESPBuffer::when_reseted()
{
    _unparsed_offset = 0;
}

size_t RESPBuffer::unparsed_size()
{
    auto res = top_offset() - _unparsed_offset;
    if (res < 0)
        return 0;
    return res;
}

const char *RESPBuffer::unparsed_data()
{
    return data() + _unparsed_offset;
}

void RESPBuffer::manage_mem()
{
    if (_unparsed_offset == top_offset()) {
        reset(true);
        return;
    }

    if (size_reserved() > size_filled() * 2)
        return;

    if ((top_offset() - _unparsed_offset) > _unparsed_offset)
        return;

    memcpy(vdata(), data() + _unparsed_offset, top_offset() - _unparsed_offset);
    change_data_top(top_offset() - _unparsed_offset);
    _unparsed_offset = 0;
}
