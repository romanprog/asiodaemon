#include "DnsBuffer.hpp"
#include "DnsDataTypes.hpp"
#include <cstring>
#include <iostream>

DnsBuffer::DnsBuffer()
{

}

bool DnsBuffer::create_dns_request(const std::string &name, DnsQType qtype)
{
    // Resrt buffer before request creating.
    reset();
    // Generate random number for ident DNS request.
    uint_to_DNSchar(_request.header.ident, rand_DNS_qid());
    // Set questions count to 1;
    uint_to_DNSchar(_request.header.QDcount, 1);
    // Set type of request.
    uint_to_DNSchar(_request.sufix.QType, static_cast<unsigned> (qtype));
    // Ask server to pursue the query recursively.
    set_DNS_flag(_request.header.flags, Dnsflag::RD, true );

    switch (qtype) {
    case DnsQType::A:
        domain_to_DNSstr(name, _request.DNS_name);

        break;
    case DnsQType::MX:
        domain_to_DNSstr(name, _request.DNS_name);

        break;
    case DnsQType::PTR:
        ip_to_Addr_arpa(name, _request.DNS_name);

        break;
    default:

        break;
    }

    release(sizeof(_request.header) + _request.DNS_name.size() + sizeof (_request.sufix));
    memcpy(data_top(), &_request.header, sizeof(_request.header));
    accept(sizeof(_request.header));

    memcpy(data_top(), _request.DNS_name.data(), _request.DNS_name.size());
    accept(_request.DNS_name.size());

    memcpy(data_top(), &_request.sufix, sizeof(_request.sufix));
    accept(sizeof(_request.sufix));

    return true;
}

std::string DnsBuffer::reqest_data_str()
{
    std::string res(data(), _top_offset);
    return std::move(res);
}

size_t DnsBuffer::calculate_mem()
{
    size_t block_size {max_DNS_pkg_size};
    size_t reserve_bl_count {1};

    return ((_top_offset + size_filled()) / block_size + reserve_bl_count) * block_size;
}

void DnsBuffer::when_new_data_acc(size_t bytes_readed)
{
    std::string tst(data(), _top_offset);
}

void DnsBuffer::when_reseted()
{

}


