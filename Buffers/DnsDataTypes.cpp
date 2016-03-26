#include "DnsDataTypes.hpp"
#include "../HelpfulCodes/HStrings.hpp"

#include <arpa/inet.h>
#include <algorithm>
#include <time.h>

namespace {

// Return length of domain section or 0 if it's pointer to it in DNS package.
uint8_t d_section_lenth(const char * &cursor)
{
    // Check 7th and 8th bit.
    // If bits are set - 6 bits of this byte and next byte is pointer to next section of domain name.
    // Pointer - offset in DNS package.

    if ((*cursor & (3 << 6)) == (3 << 6))
        return 0;

    return static_cast<uint8_t>(*cursor);
}

// Return offset of next domain section begining (see d_section_lenth description).
uint16_t d_section_offset(const char * &cursor)
{
    char offset[2];
    memcpy(offset, cursor, sizeof(uint16_t));

    // Set 7th and 8th bits to 0.
    offset[0] = offset[0] & 63;
    cursor += sizeof(uint16_t);
    return dns::DnsUtils::buff_read<uint16_t>(offset);
}


} // namespase
namespace dns {


void DnsUtils::write_step_qdn_to_buff(const std::string &dname, char *&_res)
{
    for (auto & part : hstrings::splited(dname, '.'))
    {
        *_res = static_cast<char>(part.size());
        ++_res;
        memcpy(_res, part.data(), part.size());
        _res += part.size();
    }
    *_res = '\0';
    ++_res;
}

void DnsUtils::set_flag(char *ch, const DnsFlag flag_num)
{
    auto bit_num = static_cast<unsigned>(flag_num);
    // Set flag (reverted bitmap).
    if (bit_num <= 7)
        ch[0] = ch[0] | 1 << (7-bit_num);
    else
        ch[1] = ch[1] | 1 << (7-(bit_num-8));
}

uint16_t DnsUtils::get_error(const char *flag)
{
    return static_cast<uint16_t>(flag[1] & 15);
}

void DnsUtils::buff_step_read_qdn(const char *dns_pkg, const char * &cursor, std::string &result)
{
    size_t sec_ln = d_section_lenth(cursor);

    // (cls == '\0') - end of domain name.
    if (!*cursor) {
        // Trim final "." symbol.
        result.pop_back();
        // Sen cursor to firct byte of next part.
        ++cursor;
        return;
    }
    // Read sectoin.
    if (sec_ln) {
        ++cursor;
        for (int i = 0; i < sec_ln; ++i, ++cursor)
            result += *cursor;
        result+='.';
        buff_step_read_qdn(dns_pkg, cursor, result);
        return;
    } else {
        // sec_ln == 0. We have a pointer to next section.
        // Do recursively using temp pointer.
        // Set temporary  pointer to next section begin.
        const char * curs_tmp = dns_pkg + d_section_offset(cursor);;
        buff_step_read_qdn(dns_pkg, curs_tmp, result);
    }
    return;
}

bool DnsUtils::get_flag(const char * ch , const DnsFlag flag_num)
{
    auto bit_num = static_cast<unsigned>(flag_num);

    if (bit_num <= 7)
        return ch[0] & 1 << bit_num;

    return ch[1] & 1 << (bit_num - 8);
}

std::string DnsUtils::ip_to_arpa(const std::string &ip)
{
    std::vector<std::string> octets;
    hstrings::split(ip, octets, '.');
    if (octets.size() != 4)
        return std::string();

    std::string res;

    auto riter = octets.rbegin();
    for (;riter != octets.rend(); ++riter)
        res += *riter + ".";

    res += "in-addr.arpa";

    return res;
}

uint16_t DnsUtils::rand_qid()
{
    srand(time(NULL));
    // Random 16 bit unsigned id, but not 0.
    return rand() % 65534 + 1;
}

std::string DnsUtils::ip_step_read(const char *&cursor)
{
    std::string res;
    for (int i = 0; i < 4; ++i)
        res += std::to_string(DnsUtils::buff_step_read<uint8_t>(cursor)) + ".";

    res.pop_back();
    return res;
}

bool DnsUtils::is_ip_v4(const std::string &ip)
{
    struct sockaddr_in sa;
    return inet_pton(AF_INET, ip.c_str(), &(sa.sin_addr))!=0;
}

bool DnsUtils::is_fqdn(const std::string &name)
{
    if (name.empty())
        return false;

    if (std::find_if(name.begin(), name.end(), [](char c) { return (!std::isalnum(c) && c != '-' && c != '.' ) ; }) != name.end())
        return false;

    std::vector<std::string> vec_tmp;
    hstrings::split(name, vec_tmp, '.');

    if (vec_tmp.size() < 2)
        return false;

    for (std::string & s : vec_tmp)
        if (s.empty())
            return false;

    return true;
}

bool DnsRequest::gen_request(const std::string &name, DnsQType t)
{
    // Validate @name.
    if (t == DnsQType::PTR && !DnsUtils::is_ip_v4(name))
        return false;
    else if (!DnsUtils::is_fqdn(name))
        return false;

    header.id = DnsUtils::rand_qid();
    // Set to false (request).
    header.f_req_resp = false;
    // Count of query.
    header.q_count = 1;
    header.f_recursive_d = true;
    DnsPkgQuery _q_tmp;
    _q_tmp.name = name;
    _q_tmp.type = t;
    qlist.push_back(_q_tmp);

    return true;
}

size_t DnsRequest::get_id() const
{
    return header.id;
}

size_t DnsRequest::buff_fill(void *buffer)
{
    if (!header.q_count)
        return 0;

    char * cursor = static_cast<char *>(buffer);
    char * first = cursor;

    // ==== 1) Fill package header ====
    // Create matrix of header and asociate it with buffer begin.
    DnsPkgHeaderMap * dns_header_map = reinterpret_cast<DnsPkgHeaderMap *> (buffer);
    // Write data directly into buffer.
    DnsUtils::buff_write<uint16_t>(dns_header_map->ident, header.id);
    DnsUtils::buff_write<uint16_t>(dns_header_map->QDcount, header.q_count);

    // Set to 0.
    DnsUtils::buff_write<uint16_t>(dns_header_map->ANcount, 0);
    DnsUtils::buff_write<uint16_t>(dns_header_map->ARcount, 0);
    DnsUtils::buff_write<uint16_t>(dns_header_map->NScount, 0);

    // Fill flags with 0.
    DnsUtils::buff_write<uint16_t>(dns_header_map->flags, 0);

    // Set flags.
    DnsUtils::set_flag(dns_header_map->flags, DnsFlag::RD);

    cursor += sizeof(DnsPkgHeaderMap);

    // ==== 2) Transform symbolyc domain name (or IP) to dns pkg format.
    // Create queries list.
    for (auto & q : qlist) {
        // Check if query type is DnsQType::PTR - convert ip to addr.arpa type and then transform it to DNS format.
        // If DnsQType - A or MX - name is QDN domain name.
        // q.type == DnsQType::PTR ? q.name : DnsUtils::ip_to_Addr_arpa_r(q.name)
        DnsUtils::write_step_qdn_to_buff(q.type == DnsQType::PTR ? DnsUtils::ip_to_arpa(q.name): q.name , cursor);
        // Set 2 bytes type of query.
        DnsUtils::buff_step_write<uint16_t>(cursor, static_cast<uint16_t>(q.type));
        // Set 2 bytes class of query.
        DnsUtils::buff_step_write<uint16_t>(cursor, static_cast<uint16_t>(q.cls));
    }
    return static_cast<size_t>(cursor - first);
}

bool DnsRespond::parse_respond(const void *buffer, uint16_t req_id)
{
    const char * cursor = static_cast<const char *>(buffer);
    const char * const buff_begin = static_cast<const char * const>(buffer);

    const DnsPkgHeaderMap * dns_header_map = reinterpret_cast<const DnsPkgHeaderMap *> (buffer);

    header.id = DnsUtils::buff_read<uint16_t>(dns_header_map->ident);

    // Is this respond to appropriate request?
    if (req_id && (req_id != header.id))
        return false;
    // Get error status from DNS flag.
    error = DnsUtils::get_error(dns_header_map->flags);

    // Server return error.
    if (error)
        return false;

    header.q_count = DnsUtils::buff_read<uint16_t>(dns_header_map->QDcount);
    header.a_count = DnsUtils::buff_read<uint16_t>(dns_header_map->ANcount);

    cursor += sizeof(DnsPkgHeaderMap);

    for (int i = 0; i < header.q_count; ++i) {
        DnsPkgQuery q_tmp;
        DnsUtils::buff_step_read_qdn(buff_begin, cursor, q_tmp.name);
        q_tmp.type = static_cast<DnsQType>(DnsUtils::buff_step_read<uint16_t>(cursor));
        q_tmp.cls = DnsUtils::buff_step_read<uint16_t>(cursor);
        qlist.push_back(q_tmp);
    }

    for (int i = 0; i < header.a_count; ++i) {
        DnsPkgAnswer a_tmp;
        DnsUtils::buff_step_read_qdn(buff_begin, cursor, a_tmp.req_name);
        a_tmp.type = static_cast<DnsQType>(DnsUtils::buff_step_read<uint16_t>(cursor));
        a_tmp.cls = DnsUtils::buff_step_read<uint16_t>(cursor);
        a_tmp.ttl = DnsUtils::buff_step_read<uint32_t>(cursor);
        a_tmp.dsize = DnsUtils::buff_step_read<uint16_t>(cursor);

        switch (a_tmp.type) {
        case DnsQType::A:
            a_tmp.answer = DnsUtils::ip_step_read(cursor);
            break;
        case DnsQType::PTR:
            a_tmp.preference = DnsUtils::buff_step_read<uint16_t>(cursor);
            DnsUtils::buff_step_read_qdn(buff_begin, cursor, a_tmp.answer);
            break;
        case DnsQType::MX:
            DnsUtils::buff_step_read_qdn(buff_begin, cursor, a_tmp.answer);
            break;
        }
        alist.push_back(a_tmp);
    }
    return true;
}

std::vector<DnsPkgAnswer> DnsRespond::get_answers_list()
{
    return alist;
}

}
