#ifndef DNSBUFFER_HPP
#define DNSBUFFER_HPP

#include "../BufferBase/ParsingBuffAbstract.hpp"
#include "DnsTypes.hpp"
#include "DnsUtils.hpp"

#include <string>
#include <vector>

class DnsBuffer : public BuffAbstract
{
public:
    DnsBuffer();

    bool prepare_for_request(const std::string & name, dns::DnsQType t);

    bool prepare_for_respond();

    bool read_respond(size_t bytes_readed);

    dns::DnsError get_error() const
    {
        return err;
    }

    dns::DnsRespond & get_respond();

    void clear();

private:

    virtual size_t calculate_mem() override;
    virtual void when_new_data_acc(size_t bytes_readed) override;
    dns::DnsError err;

    bool redy_for_respond {false};
    dns::DnsRequest _request;
    dns::DnsRespond _respond;
};


#endif // DNSBUFFER_HPP
