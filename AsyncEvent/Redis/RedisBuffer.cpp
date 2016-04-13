#include "RedisBuffer.hpp"

#include "../../Logger/Logger.hpp"

#include <cstring>
#include <iostream>

RedisBuffer::RedisBuffer()
     :_respond_ptr(std::make_unique<redis::RespData>())
{ }


redis::RespDataPtr RedisBuffer::withdraw_respond()
{
    return std::move(_respond_ptr);
}

void RedisBuffer::when_new_data_acc(size_t bytes_readed)
{
    if (!_parsed_offset) {
        if (_read_data(*_respond_ptr, data()))
            comlated = true;
    }
    else {
        if (_fill_array(*_respond_ptr))
            comlated = true;
    }
    int dbg = 1;
    return;
}

size_t RedisBuffer::calculate_mem()
{
    size_t reserve_bl_count {1};
    size_t _expected_part_size {100}; // Tmp value.
    return ((top_offset() + size_filled()) / _expected_part_size + reserve_bl_count) * _expected_part_size;
}

void RedisBuffer::when_reseted()
{
    _respond_ptr->reset();
    comlated = false;
    _parsed_offset = 0;
}

size_t RedisBuffer::unparsed_size()
{
    return top_offset() - _parsed_offset;
}

bool RedisBuffer::_read_simple_string(redis::RespData &target, const char * cursor, size_t sz)
{
    sz -= 3;
    size_t i;
    for (i = 0; cursor[i] != '\r'; ++i)
    {
        if (!(sz - i + 1))
            return false;
    }

    target.sres.assign(cursor, i);
    _parsed_offset = (cursor - static_cast<const char *> (vdata())) + 2;
    log_debug("%", target.ires);
    return true;
}

bool RedisBuffer::_read_integer(redis::RespData &target, const char *cursor, size_t sz)
{
    sz -= 3;

    int i {0};
    target.ires = 0;
    while (*cursor != '\r')
    {
        target.ires = (target.ires * 10) + (*cursor - '0');
        ++i; ++cursor;
        if (!(sz - i + 1))
            return false;
    }
    _parsed_offset = (cursor - static_cast<const char *> (vdata())) + 2;
    log_debug("%", target.ires);
    return true;
}

bool RedisBuffer::_read_bulk_string(redis::RespData &target, const char *cursor, size_t sz)
{
    sz -= 3;
    size_t bulk_size {0};

    // $-1\r\n. Empty bulk string.
    if (*cursor == '-')
        return true;

    int i {0};

    while (*cursor != '\r')
    {
        bulk_size = (bulk_size * 10) + (*cursor - '0');
        ++i; ++cursor;
        if (!(sz - i))
            return false;
    }

    if (sz - i < bulk_size + 2)
        return false;

    cursor += 2;
    target.sres.assign(cursor, bulk_size);
    _parsed_offset = (cursor - data()) + bulk_size + 2;
    return true;

}

bool RedisBuffer::_init_array(redis::RespData &target, const char *cursor, size_t sz)
{
    sz -= 3;
    size_t array_size {0};

    // *0\r\n. Empty array.
    if (*cursor == '0')
        return true;

    int i {0};

    while (*cursor != '\r')
    {
        array_size = (array_size * 10) + (*cursor - '0');
        ++i; ++cursor;
        if (!(sz - i + 1))
            return false;
    }

    if (!array_size)
        return true;

    target.ares.resize(array_size);
    _parsed_offset = (cursor - data()) + 2;

    if (!_fill_array(target)) {
        _incompl_arr = true;
        return false;
    }

    return true;

}

bool RedisBuffer::_fill_array(redis::RespData &target)
{
    if (target.ares.empty())
        return true;

    if (_parsed_offset >= top_offset())
        return false;

//    for (auto & un : target.ares)
    for (int i = 0; i < target.ares.size(); ++i)
    {
        int deb0 = 1;
        if (target.ares[i].type == redis::RespType::array)
        {
            if (!_fill_array(target.ares[i]))
                return false;

            continue;
        }
        else if (target.ares[i].type != redis::RespType::empty)
            continue;

        if (!_read_data(target.ares[i], data() + _parsed_offset))
            return false;

        int deb = 1;

    }
    return true;
}

bool RedisBuffer::_read_data(redis::RespData &target, const char *cursor)
{

    if (unparsed_size() < 4)
        return false; // Incomplate reply.

    switch (*cursor) {
    case '+':
    {
        // Simple string. Simple parsing.
        target.type = redis::RespType::simple_str;

        if (_read_simple_string(target, ++cursor, unparsed_size()))
            return true;

        break;
    }
    case '-':
    {
        // Simple string with error message.
        target.type = redis::RespType::error_str;

        if (_read_simple_string(target, ++cursor, unparsed_size()))
            return true;

        break;
    }
    case ':':
    {
        // Simple string with error message.
        target.type = redis::RespType::integer;

        if (_read_integer(target, ++cursor, unparsed_size()))
            return true;

        break;
    }
    case '$':
    {
        // Bulk string.
        target.type = redis::RespType::bulk_str;

        if (_read_bulk_string(target, ++cursor, unparsed_size()))
            return true;

        break;
    }
    case '*':
        // Array.
        target.type = redis::RespType::array;
        if (_init_array(target, ++cursor, unparsed_size()))
            return true;

        break;
    }

    return false;
}



