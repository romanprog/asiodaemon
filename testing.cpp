#include <iostream>
#include <bitset>
#include <string>
#include <vector>
#include <sstream>
#include <algorithm>
#include <iterator>
#include <asio.hpp>

#include "HelpfulCodes/HStrings.hpp"

using namespace std;

// You could also take an existing vector as a parameter.
std::string _ip_to_Addr_arpa(const std::string &ip)
{
    std::string res;
    std::vector<std::string> octets(hstrings::split(ip, '.'));
    auto riter = octets.rbegin();
    for (;riter != octets.rend(); ++riter)
        res += *riter + ".";
    res += "in-addr.arpa";
    return std::move(res);

}

void uint32_to_DNSchar(unsigned char *ch, const uint32_t fr)
{
    ch[0] = static_cast<unsigned char>(fr / (256 * 256 * 256));
    ch[1] = static_cast<unsigned char>(fr / (256 * 256));
    ch[2] = static_cast<unsigned char>(fr / 256);
    ch[2] = static_cast<unsigned char>(fr % 256);
}

uint32_t DNSchar_to_uint32(const unsigned char * const ch)
{

    uint32_t res = static_cast<unsigned>(ch[0])*256*256*256;
    res += static_cast<unsigned>(ch[1])*256*256;
    res += static_cast<unsigned>(ch[2])*256;
    res += static_cast<unsigned>(ch[3]);
    return res;
}
int main(int argc, char **argv) {

    cout << _ip_to_Addr_arpa("212.42.67.73") << endl;
    cout << sizeof(unsigned int) << endl;
    unsigned char u32[4];
    uint32_t val = 65536;
    uint32_to_DNSchar(u32, val);
    cout << DNSchar_to_uint32(u32) << endl;
}
