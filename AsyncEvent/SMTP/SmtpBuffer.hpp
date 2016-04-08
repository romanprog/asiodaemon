#ifndef AEVBUFFER_HPP
#define AEVBUFFER_HPP

#include "../BufferBase/ParsingBuffAbstract.hpp"
#include "../AEvBase/AEventUtilBase.hpp"
#include "SmtpTypes.hpp"

#include <string>
#include <vector>
#include <queue>


class SmtpCmdBuffer : public ParsingBuffAbstract
{
public:
    using SmtpCmdCallback = std::function<void ()>;
    SmtpCmdBuffer();

    void clear();
    std::string get_line();
    bool is_empty();
    size_t list_size() const;


private:

    virtual void when_have_new_part(const size_t begin_offset, const size_t size) override;

    std::string parsed_cmd;
    std::queue<std::string> lines_list;
};

class SmtpDataBuffer : public ParsingBuffAbstract
{
public:
    using SmtpDataCallback = std::function<void ()>;
    SmtpDataBuffer();

    void clear();
    std::string get_data();
    bool is_redy();

private:

    virtual void when_have_new_part(const size_t begin_offset, const size_t size) override;
    size_t data_end_offset {0};
};


#endif // AEVBUFFER_HPP
