#ifndef REDISBUFFER_HPP
#define REDISBUFFER_HPP

#include "../BufferBase/BuffAbstract.hpp"
#include "RedisTypes.hpp"

#include <string>

/// ///////////////////// Write Buffer //////////////////////////////
/// \brief The QueryBuffer class
/// Fast simple redis sending buffer (query buffer).
/// Supports pipeling.
/// /////////////////////////////////////////////////////////////////

class RedisQueryBuff : public BuffAbstract
{
public:
    RedisQueryBuff();
    // Return true if all contained data already sended (and confirmed).
    bool nothing_to_send();
    // Confirm data part (@bytes_sended size) sending.
    void sending_report(size_t bytes_sended);
    // Pointer to begining of new (not sended) data
    const char * new_data();
    // Size of new (not sended) data.
    size_t new_data_size();
    // Add new query tu buffer line.
    bool add_query(const std::string &query);

private:
    // Memory management: cleaning, fast reset, data transfer on free sites
    // to avoid the appearance of a new memory.
    void manage_mem();
    size_t _sended_offset {0};
};


#endif // REDISBUFFER_HPP
