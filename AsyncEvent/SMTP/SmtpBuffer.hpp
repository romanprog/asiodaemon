#ifndef AEVBUFFER_HPP
#define AEVBUFFER_HPP

#include "../BufferBase/ParsingBuffAbstract.hpp"
#include "../AEvBase/AEventUtilBase.hpp"
#include "SmtpTypes.hpp"

#include <string>
#include <vector>
#include <queue>


class SmtpBuffer : public ParsingBuffAbstract
{
public:
    using SmtpCmdCallback = std::function<void ()>;
    SmtpBuffer();

    std::string get_line();
    bool have_new_line();


private:

    virtual void when_have_new_part(const size_t begin_offset, const size_t size) override;
    virtual void when_parsed(unsigned new_parts_count) override;

    std::string parsed_cmd;
    bool have_line {false};
    std::queue<std::string> lines_list;
};


#endif // AEVBUFFER_HPP
