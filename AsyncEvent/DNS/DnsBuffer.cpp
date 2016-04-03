#include "DnsBuffer.hpp"
#include "DnsDataTypes.hpp"
#include <cstring>
#include <iostream>

DnsBuffer::DnsBuffer()
{

}

bool DnsBuffer::prepare_for_request(const std::string &name, dns::DnsQType qtype)
{
    // Resrt buffer before request creating.
    reset();

    // Create DNS request pkg.
    if (!_request.gen_request(name, qtype)) {
        error = 3;
        return false;
    }

    // Release buffer.
    release(dns::max_pkg_size);
    // Write DNS reqest to buffer.
    size_t req_size =_request.buff_fill(vdata());

    if (!req_size) {
        error = 2;
        return false;
    }

    accept(req_size);
    error = 0;
    return true;
}


bool DnsBuffer::prepare_for_respond()
{
    if (error)
        return false;

    reset();
    release(dns::max_pkg_size);
    redy_for_respond = true;
    return true;
}

bool DnsBuffer::read_respond(size_t bytes_readed)
{
    if (!redy_for_respond)
        return false;

    accept(bytes_readed);

    if (!_respond.parse_respond(vdata(), _request.get_id())) {
        error = 1;
        reset();
        return false;
    }
    error = 0;
    redy_for_respond = false;
    return true;
}

int DnsBuffer::get_error() const
{
    return error;
}

dns::DnsRespond DnsBuffer::get_respond()
{
    return _respond;
}

void DnsBuffer::clear()
{
    reset();
    _request = dns::DnsRequest();
    _respond = dns::DnsRespond();
    error = 100;
}

size_t DnsBuffer::calculate_mem()
{
    size_t block_size {dns::max_pkg_size ? dns::max_pkg_size : 1};
    size_t reserve_bl_count {1};

    return ((top_offset() + size_filled()) / block_size + reserve_bl_count) * block_size;
}

void DnsBuffer::when_new_data_acc(size_t bytes_readed)
{

}



