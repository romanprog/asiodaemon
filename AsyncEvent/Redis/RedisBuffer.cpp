#include "RedisBuffer.hpp"


RedisQueryBuff::RedisQueryBuff()
{

}

bool RedisQueryBuff::nothing_to_send()
{
    return top_offset() <= _sended_offset;
}

void RedisQueryBuff::sending_report(size_t bytes_sended)
{
    _sended_offset += bytes_sended;

    if (nothing_to_send())
        manage_mem();
}

const char *RedisQueryBuff::new_data()
{
    return data() + _sended_offset;
}

size_t RedisQueryBuff::new_data_size()
{
    return top_offset() - _sended_offset;
}

bool RedisQueryBuff::add_query(const std::string &query)
{
    *this << query;
    *this << "\r\n";

    return true;
}

void RedisQueryBuff::manage_mem()
{
    reset(true);
    _sended_offset = 0;
}
