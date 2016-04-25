#ifndef RESPBUFFER_HPP
#define RESPBUFFER_HPP

#include "../BufferBase/PBufferAbstract.hpp"
#include "RedisTypes.hpp"

#include <string>

class RESPBuffer;

using RESPBufferPtr = std::shared_ptr<RESPBuffer>;

class RESPBuffer : public BuffAbstract
{
public:
    RESPBuffer();


private:
    friend class RESProto;
    bool _incompl_arr {false};
    size_t _unparsed_offset {0};
    bool _comlated {false};

    virtual void when_reseted();
    size_t unparsed_size();
    const char * unparsed_data();
    void manage_mem();

};


#endif // RESPBUFFER_HPP




