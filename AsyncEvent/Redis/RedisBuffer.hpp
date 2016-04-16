#ifndef REDISBUFFER_HPP
#define REDISBUFFER_HPP
// Additional error hendling.
// #define ADDITIONAL_ERROR_CHECK

#include "../BufferBase/PBufferAbstract.hpp"
#include "RedisTypes.hpp"

#include <string>
#include <vector>

class RedisBuffer : public BuffAbstract
{
public:
    RedisBuffer();

    std::string error_msg() const;
    bool is_complate();
    bool have_error();
    bool parse_one(redis::RespData & respond);

private:
    virtual void when_new_data_acc(size_t bytes_readed) override;
    virtual size_t calculate_mem() override;
    virtual void when_reseted();

    size_t unparsed_size();

    inline bool _read_simple_string(redis::RespData &target, const char *cursor, size_t sz);
    inline bool _read_integer(redis::RespData &target, const char *cursor, size_t sz);

    bool _read_bulk_string(redis::RespData &target, const char *cursor, size_t sz);
    bool _init_array(redis::RespData &target, const char *cursor, size_t sz);
    bool _fill_array(redis::RespData &target);
    bool _read_data(redis::RespData &target, const char *cursor);

    inline void parsing_error_hendler();
    void manage_mem();

    std::string _err_message;
    bool _error_status {false};
    bool _comlated {false};
    bool _incompl_arr {false};
    size_t _unparsed_offset {0};

};


#endif // REDISBUFFER_HPP
