#ifndef DNSBUFFER_HPP
#define DNSBUFFER_HPP

#include "Abstract/ParsingBuffAbstract.hpp"
#include "DnsDataTypes.hpp"

#include <string>
#include <vector>

class DnsBuffer : public BuffAbstract
{
public:
    DnsBuffer();
    bool create_dns_request(const std::string & name, DnsQType qtype);
    std::string reqest_data_str();

private:

    virtual size_t calculate_mem() override;
    virtual void when_new_data_acc(size_t bytes_readed) override;
    virtual void when_reseted() override;

    DnsRequest _request;
};


#endif // DNSBUFFER_HPP
