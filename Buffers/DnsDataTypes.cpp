#include "DnsDataTypes.hpp"
#include "../HelpfulCodes/HStrings.hpp"

#include <time.h>

namespace {

// Return length of domain section or 0 if it's pointer ()
uint8_t d_section_lenth(const char c)
{
    // Check 7th and 8th bit.
    // If bits are set - 6 bits of this byte and next byte is pointer to next section of domain name.
    // Pointer - offset in DNS package.
    if ((c & (3 << 6)) == (3 << 6))
        return 0;

    return static_cast<uint8_t>(c);
}

// Return offset of next domain section begining (see d_section_lenth description).
uint16_t d_section_offset(const unsigned char * const offset_data)
{
    // Clear 7th and 8th bits.
    unsigned char offset[2];
    memcpy(offset, offset_data, 2);
    offset[0] = offset[0] & 63;
    return DnsUtils::DNSchar_to_uint16(offset);
}

} // namespase

// Convert uitn value to 2bytes format for DNS packets.
void DnsUtils::uint16_to_DNSchar(unsigned char *ch, const uint16_t fr)
{
    ch[0] = static_cast<unsigned char>(fr / 256);
    ch[1] = static_cast<unsigned char>(fr % 256);
}

// Convert 2 bytes from DNS packet to uint format.
uint16_t DnsUtils::DNSchar_to_uint16(const unsigned char * const ch)
{
    uint16_t res = static_cast<unsigned>(ch[0])*256 + static_cast<unsigned>(ch[1]);
    return res;
}

void DnsUtils::uint32_to_DNSchar(unsigned char *ch, const uint32_t fr)
{
    ch[0] = static_cast<unsigned char>(fr / (256 * 256 * 256));
    ch[1] = static_cast<unsigned char>(fr / (256 * 256));
    ch[2] = static_cast<unsigned char>(fr / 256);
    ch[2] = static_cast<unsigned char>(fr % 256);
}

uint32_t DnsUtils::DNSchar_to_uint32(const unsigned char * const ch)
{

    uint32_t res = static_cast<unsigned>(ch[0])*256*256*256;
    res += static_cast<unsigned>(ch[1])*256*256;
    res += static_cast<unsigned>(ch[2])*256;
    res += static_cast<unsigned>(ch[3]);
    return res;
}

void DnsUtils::domain_to_DNSstr(const std::string &dname, std::string &_res)
{

    _res.clear();

    for (auto & part : hstrings::split(dname, '.'))
    {
        _res += static_cast<unsigned char>(part.size());
        _res += part;
    }

    _res += '\0';

}

std::string DnsUtils::domain_to_DNSstr_r(const std::string &dname)
{
    std::string res;
    DnsUtils::domain_to_DNSstr(dname, res);
    return std::move(res);

}

void DnsUtils::set_DNS_flag(unsigned char *ch, const DnsFlag flag_num, const bool flag_value)
{
    auto bit_num = static_cast<unsigned>(flag_num);
    // Set flag (reverted bitmap).
    if (bit_num <= 7)
        ch[0] = ch[0] | 1 << (7-bit_num);
    else
        ch[1] = ch[1] | 1 << (7-(bit_num-8));

}

size_t DnsUtils::read_domain(const void *dns_pkg, size_t start_pos, std::string &result)
{
    const unsigned char * cursor = static_cast<const unsigned char *> (dns_pkg) + start_pos;
    size_t sec_ln = d_section_lenth(*cursor);

    size_t end_offset {0};

    // (cls == '\0') - end of domain name.
    if (!*cursor) {
        result.resize(result.size() - 1);
        return 1;
    }
    // Read sectoin.
    if (sec_ln) {
        ++cursor;
        end_offset += sec_ln + 1;
        for (int i = 0; i < sec_ln; ++i, ++cursor)
            result += *cursor;
        result+='.';
        end_offset += read_domain(dns_pkg, cursor - static_cast<const unsigned char *>(dns_pkg), result);
        return end_offset;
    } else {

    // sec_ln == 0. We have a pointer to next section.
    // Do recursively.
        end_offset += 2;
        size_t sec_offset = d_section_offset(cursor);
        read_domain(dns_pkg, sec_offset, result);
    }
    return end_offset;
}

bool DnsUtils::get_DNS_flag( unsigned char *const ch , const DnsFlag flag_num)
{
    auto bit_num = static_cast<unsigned>(flag_num);

    if (bit_num <= 7)
        return ch[0] & 1 << bit_num;

    return ch[1] & 1 << (bit_num - 8);
}

bool DnsUtils::ip_to_Addr_arpa(const std::string &ip, std::string & res)
{

    std::vector<std::string> octets(hstrings::split(ip, '.'));
    if (octets.size() != 4)
        return false;

    auto riter = octets.rbegin();
    for (;riter != octets.rend(); ++riter)
        res += *riter + ".";

    res += "in-addr.arpa";
    return true;

}

std::string DnsUtils::ip_to_Addr_arpa_r(const std::string &ip)
{
    std::string res;
    DnsUtils::ip_to_Addr_arpa(ip, res);
    return std::move(res);
}

uint16_t DnsUtils::rand_DNS_qid()
{
    srand(time(NULL));
    return rand() % 65535;
}
