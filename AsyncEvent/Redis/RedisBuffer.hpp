#ifndef REDISBUFFER_HPP
#define REDISBUFFER_HPP

#include "../BufferBase/PBufferAbstract.hpp"
#include "RedisTypes.hpp"

#include <string>
#include <vector>

class RedisBuffer : public BuffAbstract
{
public:
    RedisBuffer();

    std::string get_error_text() const
    {
        return err_text;
    }

    redis::RespDataPtr withdraw_respond();

    void clear();

private:
    virtual void when_new_data_acc(size_t bytes_readed) override;
    virtual size_t calculate_mem() override;

    size_t unparsed_size();

    inline bool _read_simple_string(redis::RespData &target, const char *cursor, size_t sz);
    inline bool _read_integer(redis::RespData &target, const char *cursor, size_t sz);

    bool _read_bulk_string(redis::RespData &target, const char *cursor, size_t sz);
    bool _init_array(redis::RespData &target, const char *cursor, size_t sz);
    bool _fill_array(redis::RespData &target);
    bool _read_data(redis::RespData &target, const char *cursor);

    std::string err_text;
    bool comlated {false};
    bool _incompl_arr {false};
    redis::RespDataPtr _respond_ptr;
    size_t _parsed_offset {0};

};


#endif // REDISBUFFER_HPP
