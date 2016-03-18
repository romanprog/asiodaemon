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
    const std::string & answer();
    const std::string get_answer();
    bool smtp_error {false};
    bool abort{false};
    bool have_answer {true};
    bool waiting_for_command {false};

private:

    virtual void when_have_new_part(const size_t begin_offset, const size_t size) override;
    virtual void when_parsed(unsigned new_parts_count) override;

    std::string primary_domain {"my-home.com"};
    std::string smtp_answer;
};

}
#endif // AEVBUFFER_HPP
