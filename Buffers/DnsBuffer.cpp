#include "DnsBuffer.hpp"
#include <cstring>
#include <iostream>

namespace {

void add_dpart(std::string & dstr, const char * str, size_t size)
{
    dstr += static_cast<unsigned char>(size);
    dstr.append(str, size);
}

} // Unnamed namespace.

DnsBuffer::DnsBuffer()
{

}

void DnsBuffer::set_domain(const std::string &dname)
{
    reset();
    DnsDname.clear();
    size_t pos{0};
    size_t prev_pos{0};
    while ((pos = dname.find('.', pos+1)) != std::string::npos)
    {
        add_dpart(DnsDname, dname.c_str() + prev_pos, pos - prev_pos);
        prev_pos = pos + 1;
    }
    if (prev_pos < dname.size()) {
        add_dpart(DnsDname, dname.c_str() + prev_pos, dname.size() - prev_pos);
    }
    DnsDname += '\0';

    for (char c : DnsDname)
        if (c < 20)
            std::cout << static_cast<int>(c);
        else
            std::cout << c;

    std::cout << std::endl;

    release(sizeof(d_header) + DnsDname.size() + sizeof (d_sufix));

    memcpy(data_top(), &d_header, sizeof(d_header));
    accept(sizeof(d_header));

    memcpy(data_top(), DnsDname.data(), DnsDname.size());
    accept(DnsDname.size());

    memcpy(data_top(), &d_sufix, sizeof(d_sufix));
    accept(sizeof(d_sufix));
}

size_t DnsBuffer::calculate_mem()
{
    size_t block_size {512};
    size_t reserve_bl_count {1};

    return ((_top_offset + size_filled()) / block_size + reserve_bl_count) * block_size;
}

void DnsBuffer::when_new_data_acc(size_t bytes_readed)
{
    size_t d = bytes_readed;
    std::string tst(data(), _top_offset);
    for (unsigned char c : tst)
        std::cout << static_cast<int>(c) << " ";

    std::cout << std::endl;

    d = 0;
}

void DnsBuffer::when_reseted()
{

}

