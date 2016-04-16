#include "RedisBuffer.hpp"

#include "../../Logger/Logger.hpp"

#include <cstring>
#include <iostream>
#include <algorithm>

RedisBuffer::RedisBuffer()
{ }

std::string RedisBuffer::error_msg() const
{
    return _err_message;
}

bool RedisBuffer::is_complate()
{
    return _comlated;
}

bool RedisBuffer::have_error()
{
    return _error_status;
}

bool RedisBuffer::parse_one(redis::RespData &respond)
{
    _comlated = false;

    if (top_offset() <= _unparsed_offset)
        return false;

    if (!_incompl_arr) {

        respond.reset();
        if (_read_data(respond, data() + _unparsed_offset))
            _comlated = true;
    }
    else {
        if (_fill_array(respond))
            _comlated = true;
    }
    if (_comlated)
        manage_mem();

    return _comlated;
}

void RedisBuffer::when_new_data_acc(size_t bytes_readed)
{

}

size_t RedisBuffer::calculate_mem()
{
    size_t reserve_bl_count {2};
    size_t _expected_part_size {256}; // Tmp value.
    return ((top_offset() + size_filled()) / _expected_part_size + reserve_bl_count) * _expected_part_size;
}

void RedisBuffer::when_reseted()
{
//    _comlated = false;
    _unparsed_offset = 0;
}

size_t RedisBuffer::unparsed_size()
{
    return top_offset() - _unparsed_offset;
}

bool RedisBuffer::_read_simple_string(redis::RespData &target, const char * cursor, size_t sz)
{
    size_t i {0};

    while (cursor[i] != '\r')
    {
        if (!(sz - i - 2))
            return false;
        ++i;
    }

#ifdef ADDITIONAL_ERROR_CHECK
    if (cursor[i+1] != '\n') {
        parsing_error_hendler();
        return false;
    }
#endif

    target.sres.assign(cursor, i);
    _unparsed_offset = (cursor - data()) + i + 2; // 2 = "\r\n" length

    return true;
}

bool RedisBuffer::_read_integer(redis::RespData &target, const char *cursor, size_t sz)
{
    target.ires = 0;
    size_t i {0};

    while (cursor[i] != '\r')
    {
        if (!(sz - i - 2))
            return false;
        target.ires = (target.ires * 10) + (cursor[i] - '0');
        ++i;
    }

#ifdef ADDITIONAL_ERROR_CHECK
    if (cursor[i+1] != '\n') {
        parsing_error_hendler();
        return false;
    }
#endif

    target.type = redis::RespType::integer;
    _unparsed_offset = (cursor - data()) + i + 2; // 2 = "\r\n" length
    return true;
}

bool RedisBuffer::_read_bulk_string(redis::RespData &target, const char *cursor, size_t sz)
{
    // $-1\r\n. Null bulk string. (Unexisting key).
    if (*cursor == '-') {
#ifdef ADDITIONAL_ERROR_CHECK
        if (cursor[1] != '1' || cursor[2] != '\r' || cursor[3] != '\n') {
            parsing_error_hendler();
            return false;
        }
#endif
        target.isnull = true;
        _unparsed_offset = (cursor - data()) + 4;
        return true;
    }

    size_t bulk_size {0};
    size_t i {0};

    while (cursor[i] != '\r')
    {
        if (!(sz - i))
            return false;
        bulk_size = (bulk_size * 10) + (cursor[i] - '0');
        ++i;
    }

    if (sz - i < bulk_size + 4)
        return false;

#ifdef ADDITIONAL_ERROR_CHECK
    if (cursor[bulk_size + i + 3] != '\n') {
        parsing_error_hendler();
        return false;
    }
#endif

    target.sres.assign(cursor + i + 2, bulk_size);
    target.type = redis::RespType::bulk_str;
    _unparsed_offset = (cursor - data()) + bulk_size + i + 4;
    return true;

}

bool RedisBuffer::_init_array(redis::RespData &target, const char *cursor, size_t sz)
{
    // *0\r\n. Empty array.
    if (*cursor == '0') {
#ifdef ADDITIONAL_ERROR_CHECK
        if (cursor[1] != '\r' || cursor[2] != '\n') {
            parsing_error_hendler();
            return false;
        }
#endif
        target.isnull = true;
        _unparsed_offset = (cursor - data()) + 3;
        return true;
    }

    size_t array_size {0};
    size_t i {0};

    while (cursor[i] != '\r')
    {
        if (!(sz - i))
            return false;
        array_size = (array_size * 10) + (cursor[i] - '0');
        ++i;
    }

#ifdef ADDITIONAL_ERROR_CHECK
    if (cursor[i + 1] != '\n') {
        parsing_error_hendler();
        return false;
    }
#endif

    target.ares.resize(array_size);
    _unparsed_offset = (cursor - data()) + i + 2;

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

    if (_unparsed_offset >= top_offset())
        return false;

    for (int i = 0; i < target.ares.size(); ++i)
    {
        if (target.ares[i].type == redis::RespType::array)
        {
            if (!_fill_array(target.ares[i]))
                return false;

            continue;
        }
        else if (target.ares[i].type != redis::RespType::empty)
            continue;

        if (!_read_data(target.ares[i], data() + _unparsed_offset))
            return false;

    }
    return true;
}

bool RedisBuffer::_read_data(redis::RespData &target, const char *cursor)
{

    switch (*cursor) {
    case '+':
    {
        // Simple string. Simple parsing.

        if (unparsed_size() < 4)
            return false; // Incomplate reply.

        if (_read_simple_string(target, ++cursor, unparsed_size() - 1)) {
            target.type = redis::RespType::simple_str;
            return true;
        }

        break;
    }
    case '-':
    {
        if (unparsed_size() < 4)
            return false; // Incomplate reply.

        // Simple string with error message.
        if (_read_simple_string(target, ++cursor, unparsed_size() - 1)) {
            target.type = redis::RespType::error_str;
            return true;
        }

        break;
    }
    case ':':
    {
        if (unparsed_size() < 4)
            return false; // Incomplate reply.

        if (_read_integer(target, ++cursor, unparsed_size() - 1))
            return true;

        break;
    }
    case '$':
    {
        // Bulk string.
        if (unparsed_size() < 5)
            return false; // Incomplate reply.

        if (_read_bulk_string(target, ++cursor, unparsed_size() - 1)) {
            target.type = redis::RespType::bulk_str;
            return true;
        }
        break;
    }
    case '*':
        // Array.
        if (unparsed_size() < 4)
            return false; // Incomplate reply.

        if (_init_array(target, ++cursor, unparsed_size() - 1))
            return true;

        break;
    default:
        parsing_error_hendler();
    }

    return false;
}

void RedisBuffer::parsing_error_hendler()
{
    _error_status = true;
    _err_message = "Reply data parsing error.";
    log_debug(_err_message);
}

void RedisBuffer::manage_mem()
{
    if (_unparsed_offset >= top_offset()) {
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



