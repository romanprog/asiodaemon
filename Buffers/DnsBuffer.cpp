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
    DnsUtils::uint16_to_DNSchar(_request.header.ident, DnsUtils::rand_DNS_qid());
    // Set questions count to 1;
    DnsUtils::uint16_to_DNSchar(_request.header.QDcount, 1);
    // Set type of request.
    DnsUtils::uint16_to_DNSchar(_request.sufix.QType, static_cast<unsigned> (qtype));
    // Ask server to pursue the query recursively.
    DnsUtils::set_DNS_flag(_request.header.flags, DnsFlag::RD, true );


    switch (qtype) {
    case DnsQType::A:
        DnsUtils::domain_to_DNSstr(name, _request.DNS_name);
        break;
    case DnsQType::MX:
        DnsUtils::domain_to_DNSstr(name, _request.DNS_name);
        break;
    case DnsQType::PTR:
        DnsUtils::domain_to_DNSstr(DnsUtils::ip_to_Addr_arpa_r(name) , _request.DNS_name);
        break;
    default:
        return false;
        break;
    }

    _request.type = qtype;

    // Fill buffer.
    *this << _request.header;
    *this << _request.DNS_name;
    *this << _request.sufix;

    _request.size = size_filled();

    return true;
}

std::string DnsBuffer::reqest_data_str()
{
    std::string res(data(), _top_offset);
    return std::move(res);
}

bool DnsBuffer::parse_dns_respond()
{
    memcpy(&_respond.header, data(), sizeof(DnsPkgHeader));
    _data_cursor += sizeof(DnsPkgHeader);

    // This package is respond.
    if (!DnsUtils::get_DNS_flag(_respond.header.flags, DnsFlag::QR ))
        return false;
    // This is expected respond (ident num).
    if (DnsUtils::DNSchar_to_uint16(_respond.header.ident) != DnsUtils::DNSchar_to_uint16(_request.header.ident))
        return false;

    _respond.answers_count = DnsUtils::DNSchar_to_uint16(_respond.header.ANcount);
    std::string domain_name;

    _data_cursor += DnsUtils::read_domain(vdata(), _data_cursor, _respond.name);

    memcpy(&_respond.sufix, data() + _data_cursor, sizeof(DnsPkgSufix));
    _data_cursor += sizeof(DnsPkgSufix);

    for (int i = 0; i < _respond.answers_count; ++i)
    {
        if (_request.type == DnsQType::MX) {
            _data_cursor += DnsUtils::read_domain(vdata(), _data_cursor, domain_name);
            DnsAnswerEntryInfoMX _info_tmp;
            memcpy(&_info_tmp, data() + _data_cursor, sizeof(DnsAnswerEntryInfoMX));

            DnsAnswerEntry answer_tmp;

            _data_cursor += sizeof(DnsAnswerEntryInfoMX);
            answer_tmp.data_lenth = DnsUtils::DNSchar_to_uint16(_info_tmp.base_info.data_lenth);
            answer_tmp.preference = DnsUtils::DNSchar_to_uint16(_info_tmp.preference);
            _data_cursor += DnsUtils::read_domain(vdata(), _data_cursor, answer_tmp.entry);
            _respond.ansvers.push_back(answer_tmp);
        }
        if (_request.type == DnsQType::PTR) {
            _data_cursor += DnsUtils::read_domain(vdata(), _data_cursor, domain_name);
            DnsAnswerEntryInfo _info_tmp;
            DnsAnswerEntry answer_tmp;

            memcpy(&_info_tmp, data() + _data_cursor, sizeof(DnsAnswerEntryInfo));
            _data_cursor += sizeof(DnsAnswerEntryInfo);

            answer_tmp.data_lenth = DnsUtils::DNSchar_to_uint16(_info_tmp.data_lenth);

            _data_cursor += DnsUtils::read_domain(vdata(), _data_cursor, answer_tmp.entry);
            _respond.ansvers.push_back(answer_tmp);
        }
    }

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
    std::string tst(data(), _top_offset);
}

void DnsBuffer::when_reseted()
{
    // _request = DnsRequest();
}


