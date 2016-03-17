#ifndef AEVBUFFER_HPP
#define AEVBUFFER_HPP

#include "AEvBuffer/ParsingBuffAbstract.hpp"

#include <string>
#include <vector>

namespace aev {

class AEvBuffer : public ParsingBuffAbstract
{
public:
    AEvBuffer();

private:

    virtual void when_have_new_part(const size_t begin_offset, const size_t size) override;
    virtual void when_parsed(unsigned new_parts_count) override;

};

}
#endif // AEVBUFFER_HPP
