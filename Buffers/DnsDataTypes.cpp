#include "DnsDataTypes.hpp"
#include "../HelpfulCodes/HStrings.hpp"

#include <time.h>


// Convert uitn value to 2bytes format for DNS packets.
void uint_to_DNSchar(unsigned char *ch, const uint16_t fr)
{

    ch[0] = static_cast<unsigned char>(fr / 256);
    ch[1] = static_cast<unsigned char>(fr % 256);
}

// Convert 2 bytes from DNS packet to uint format.
uint16_t DNSchar_to_uint(unsigned char ch[])
{
    uint16_t res = static_cast<unsigned>(ch[0])*256 + static_cast<unsigned>(ch[1]);
    return res;
}

void domain_to_DNSstr(const std::string &dname, std::string &_res)
{

    _res.clear();

    for (auto & part : hstrings::split(dname, '.'))
    {
        _res += static_cast<unsigned char>(part.size());
        _res += part;
    }

    _res += '\0';

}

void set_DNS_flag(unsigned char *ch, const Dnsflag flag_num, const bool flag_value)
{
    auto bit_num = static_cast<unsigned>(flag_num);
    // Set flag (reverted bitmap).
    if (bit_num <= 7)
        ch[0] = ch[0] | 1 << (7-bit_num);
    else
        ch[1] = ch[1] | 1 << (7-(bit_num-8));

}

bool get_DNS_flag( unsigned char *const ch , const Dnsflag flag_num)
{
    auto bit_num = static_cast<unsigned>(flag_num);

    if (bit_num <= 7)
        return ch[0] & 1 << bit_num;

    return ch[1] & 1 << (bit_num - 8);
}

bool ip_to_Addr_arpa(const std::string &ip, std::string & res)
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

uint16_t rand_DNS_qid()
{
    srand(time(NULL));
    return rand() % 65535;
}
