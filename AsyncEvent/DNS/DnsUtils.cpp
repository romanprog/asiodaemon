#include "DnsUtils.hpp"
#include "../../HUtils/HStrings.hpp"

#include <algorithm>
#include <arpa/inet.h>

namespace dns {
namespace utils {

void write_step_qdn_to_buff(const std::string &dname, char *&_res)
{
    for (auto & part : hstrings::splitted(dname, '.'))
    {
        *_res = static_cast<char>(part.size());
        ++_res;
        memcpy(_res, part.data(), part.size());
        _res += part.size();
    }
    *_res = '\0';
    ++_res;
}

void set_flag(char *ch, const DnsFlag flag_num)
{
    auto bit_num = static_cast<unsigned>(flag_num);
    // Set flag (reverted bitmap).
    if (bit_num <= 7)
        ch[0] = ch[0] | 1 << (7-bit_num);
    else
        ch[1] = ch[1] | 1 << (7-(bit_num-8));
}

uint16_t get_error(const char *flag)
{
    return static_cast<uint16_t>(flag[1] & 15);
}

void buff_step_read_qdn(const char *dns_pkg, const char * &cursor, std::string &result)
{
    // Workers lambdas.
    // Return length of domain section or 0 if it's pointer to next section begin in DNS package.
    auto d_section_lenth = [](const char * &cursor) -> uint8_t
    {
        // Check 7th and 8th bit.
        // If bits are set - 6 bits of this byte and next byte is pointer to next section of domain name.
        // Pointer - offset in DNS package.

        if ((*cursor & (3 << 6)) == (3 << 6))
            return 0;

        return static_cast<uint8_t>(*cursor);
    };

    // Return offset of next domain section begining (see d_section_lenth description).
    // Increase cursor position (ref to pointer).
    auto d_section_offset = [](const char * &cursor) -> uint16_t
    {
        char offset[2];
        memcpy(offset, cursor, sizeof(uint16_t));

        // Set 7th and 8th bits to 0.
        offset[0] = offset[0] & 63;
        cursor += sizeof(uint16_t);
        return dns::utils::buff_read<uint16_t>(offset);
    };


    size_t sec_ln = d_section_lenth(cursor);

    // (cls == '\0') - end of domain name.
    if (!*cursor) {
        // Trim final "." symbol.
        result.pop_back();
        // Set cursor to first byte of next part.
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
        const char * curs_tmp = dns_pkg + d_section_offset(cursor);
        buff_step_read_qdn(dns_pkg, curs_tmp, result);
    }
    return;
}

bool get_flag(const char * ch , const DnsFlag flag_num)
{
    auto bit_num = static_cast<unsigned>(flag_num);

    if (bit_num <= 7)
        return ch[0] & 1 << bit_num;

    return ch[1] & 1 << (bit_num - 8);
}

std::string ip_to_arpa(const std::string &ip)
{
    std::string res;
    std::vector<std::string> octets;
    hstrings::split(ip, octets, '.');
    if (octets.size() != 4)
        return res;

    auto riter = octets.rbegin();
    for (;riter != octets.rend(); ++riter)
        res += *riter + ".";

    res += "in-addr.arpa";

    return res;
}

uint16_t rand_qid()
{
    srand(time(NULL));
    // Random 16 bit unsigned id, but not 0.
    return rand() % 65534 + 1;
}

std::string ip_step_read(const char *&cursor)
{
    std::string res;
    for (int i = 0; i < 4; ++i)
        res += std::to_string(utils::buff_step_read<uint8_t>(cursor)) + ".";

    res.pop_back();
    return res;
}

bool is_ip_v4(const std::string &ip)
{
    struct sockaddr_in sa;
    return inet_pton(AF_INET, ip.c_str(), &(sa.sin_addr))!=0;
}

bool is_fqdn(const std::string &name)
{
    if (name.empty())
        return false;

    // Search invalid symbols.
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

size_t request_buff_write(void *buffer, DnsRequest req)
{
    // ToDo exeption!!
    // Need testd when parsed invalid data.
    int todo = 1;
    if (!req.header.q_count)
        return 0;

    char * cursor = static_cast<char *>(buffer);
    char * first = cursor;

    // ==== 1) Fill package header ====
    // Create matrix of header and asociate it with buffer begin.
    DnsPkgHeaderMap * dns_header_map = reinterpret_cast<DnsPkgHeaderMap *> (buffer);
    // Write data directly into buffer.
    utils::buff_write<uint16_t>(dns_header_map->ident, req.header.id);
    utils::buff_write<uint16_t>(dns_header_map->QDcount, req.header.q_count);

    // Set to 0.
    utils::buff_write<uint16_t>(dns_header_map->ANcount, 0);
    utils::buff_write<uint16_t>(dns_header_map->ARcount, 0);
    utils::buff_write<uint16_t>(dns_header_map->NScount, 0);

    // Fill flags with 0.
    utils::buff_write<uint16_t>(dns_header_map->flags, 0);

    // Set flags.
    utils::set_flag(dns_header_map->flags, DnsFlag::RD);

    cursor += sizeof(DnsPkgHeaderMap);

    // ==== 2) Transform symbolyc domain name (or IP) to dns pkg format.
    // Create queries list.
    for (auto & q : req.qlist) {
        // Check if query type is DnsQType::PTR - convert ip to addr.arpa type and then transform it to DNS format.
        // If DnsQType - A or MX - name is QDN domain name.
        // q.type == DnsQType::PTR ? q.name : DnsUtils::ip_to_Addr_arpa_r(q.name)
        utils::write_step_qdn_to_buff(q.type == DnsQType::PTR ? utils::ip_to_arpa(q.name): q.name , cursor);
        // Set 2 bytes type of query.
        utils::buff_step_write(cursor, static_cast<uint16_t>(q.type));
        // Set 2 bytes class of query.
        utils::buff_step_write(cursor, static_cast<uint16_t>(q.cls));
    }
    return static_cast<size_t>(cursor - first);
}

DnsError respond_buff_parse(const void *buffer, DnsRespond &res, uint16_t req_id)
{
    const char * cursor = static_cast<const char *>(buffer);
    const char * const buff_begin = static_cast<const char * const>(buffer);

    const DnsPkgHeaderMap * dns_header_map = reinterpret_cast<const DnsPkgHeaderMap *> (buffer);

    res.header.id = utils::buff_read<uint16_t>(dns_header_map->ident);

    // Is this respond to appropriate request?
    if (req_id && (req_id != res.header.id))
        return DnsError::id_match_err;

    // Get error status from DNS flag.
    res.header.error = utils::get_error(dns_header_map->flags);

    // Server return error.
    if (res.header.error)
        return DnsError::resolv_err;

    res.header.q_count = utils::buff_read<uint16_t>(dns_header_map->QDcount);
    res.header.a_count = utils::buff_read<uint16_t>(dns_header_map->ANcount);

    cursor += sizeof(DnsPkgHeaderMap);

    for (int i = 0; i < res.header.q_count; ++i) {
        std::string name;
        utils::buff_step_read_qdn(buff_begin, cursor, name);
        auto type = static_cast<DnsQType>(utils::buff_step_read<uint16_t>(cursor));
        auto cls = utils::buff_step_read<uint16_t>(cursor);
        res.qlist.emplace_back(name, type, cls);
    }

    for (int i = 0; i < res.header.a_count; ++i) {
        DnsPkgAnswer a_tmp;
        utils::buff_step_read_qdn(buff_begin, cursor, a_tmp.req_name);
        a_tmp.type = static_cast<DnsQType>(utils::buff_step_read<uint16_t>(cursor));
        a_tmp.cls = utils::buff_step_read<uint16_t>(cursor);
        a_tmp.ttl = utils::buff_step_read<uint32_t>(cursor);
        a_tmp.dsize = utils::buff_step_read<uint16_t>(cursor);

        switch (a_tmp.type) {
        case DnsQType::A:
            a_tmp.answer = utils::ip_step_read(cursor);
            break;
        case DnsQType::PTR:
            utils::buff_step_read_qdn(buff_begin, cursor, a_tmp.answer);
            break;
        case DnsQType::MX:
            a_tmp.preference = utils::buff_step_read<uint16_t>(cursor);
            utils::buff_step_read_qdn(buff_begin, cursor, a_tmp.answer);
            break;
        }
        res.alist.push_back(a_tmp);
    }
    return DnsError::noerror;
}

DnsError create_request(const std::string &req_str, DnsQType t, DnsRequest &req)
{
    // Validate
    if (t == DnsQType::PTR && !utils::is_ip_v4(req_str))
        return DnsError::req_str_err;
    else if (!utils::is_fqdn(req_str))
        return DnsError::req_str_err;

    req.header.id = rand_qid();
    // Set to false (request).
    req.header.f_req_resp = false;
    // Count of query.
    req.header.q_count = 1;
    req.header.f_recursive_d = true;
    req.qlist.emplace_back(req_str, t);
    return DnsError::noerror;
}

std::map<uint16_t, std::string> get_mx_sorted(const DnsRespond &resp)
{
    std::map<uint16_t, std::string> result;
    for (auto & rec : resp.alist)
    {
        result.insert(std::pair<uint16_t, std::string>(rec.preference, rec.answer));
    }
    return result;
}

std::vector<std::string> get_rec_list(const DnsRespond &resp)
{
    std::vector<std::string> result;
    for (auto & rec : resp.alist)
    {
        result.push_back(rec.answer);
    }
    return result;
}

std::string get_rand_rec_value(const DnsRespond &resp)
{

    if (resp.alist.size() == 1)
        return resp.alist[0].answer;

    if (resp.alist.size() == 0) {
        std::string res;
        return res;
    }
    srand(time(NULL));

    return resp.alist[rand() % (resp.alist.size() - 1)].answer;

}

} // namespace utils
} //namespace dns
