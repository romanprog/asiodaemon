#ifndef AEVBUFFER_HPP
#define AEVBUFFER_HPP

#include "Abstract/ParsingBuffAbstract.hpp"
#include "../AsyncEvent/Abstract/AEventUtilBase.hpp"

#include <string>
#include <vector>


class SmtpBuffer : public aev::AEventUtilBase, public ParsingBuffAbstract
{
public:
    using SmtpCmdCallback = std::function<void ()>;
    SmtpBuffer();
    ~SmtpBuffer();
    void parse_smtp(size_t size, SmtpCmdCallback cb);
    const std::string & answer();
    const std::string get_answer();
    const std::string get_last_cmd();

    bool smtp_error {false};
    bool abort{false};
    bool have_answer {true};
    bool waiting_for_command {false};

private:

    virtual void when_have_new_part(const size_t begin_offset, const size_t size) override;
    virtual void when_parsed(unsigned new_parts_count) override;

    SmtpCmdCallback _when_have_ansver_cb;
    std::string primary_domain {"my-home.com"};
    std::string smtp_answer;
    std::string parsed_cmd;
};


#endif // AEVBUFFER_HPP
