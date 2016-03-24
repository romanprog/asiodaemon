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
    // Create DNS request pkg.
    if (!_request.gen_request(name, qtype))
        return false;

    // Release buffer.
    release(max_DNS_pkg_size);
    // Write DNS reqest to buffer.
    size_t req_size =_request.buff_fill(vdata());

    if (!req_size)
        return false;

    accept(req_size);

    return true;
}

std::string DnsBuffer::reqest_data_str()
{
    std::string res(data(), _top_offset);
    return std::move(res);
}

bool DnsBuffer::parse_dns_respond()
{

    _respond.parse_respond(vdata(), _request.get_id());
    return true;
}

size_t DnsBuffer::calculate_mem()
{
    size_t block_size {max_DNS_pkg_size ? max_DNS_pkg_size : 1};
    size_t reserve_bl_count {1};

    return ((_top_offset + size_filled()) / block_size + reserve_bl_count) * block_size;
}

void DnsBuffer::when_new_data_acc(size_t bytes_readed)
{
//    std::string tst(data(), _top_offset);
//    int i = 1;
}

void DnsBuffer::when_reseted()
{
    // _request = DnsRequest();
}


