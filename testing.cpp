#include <iostream>
#include <bitset>
#include <string>
#include <vector>
#include <sstream>
#include <algorithm>
#include <iterator>
#include <asio.hpp>

#include "HelpfulCodes/HStrings.hpp"
// #include "Buffers/DnsBuffer.hpp"

//std::string reverse_ip(std::string ip)
//{
//    size_t pos{0};
//    size_t prev_pos{0};
//    std::vector<std::string> octets;

//    while ((pos = ip.find('.', pos+1)) != std::string::npos)
//    {
//        octets[] (_res, dname.c_str() + prev_pos, pos - prev_pos);
//        prev_pos = pos + 1;
//    }
//    if (prev_pos < dname.size()) {
//        add_dpart(_res, dname.c_str() + prev_pos, dname.size() - prev_pos);
//    }
//    _res += '\0';
//}
// I don't recommend using the std namespace in production code.
// For ease of reading here.
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


int main(int argc, char **argv) {

    cout << _ip_to_Addr_arpa("212.42.67.73") << endl;
}
