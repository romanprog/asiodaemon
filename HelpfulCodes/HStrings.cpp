#include "HStrings.hpp"

#include <sstream>
#include <algorithm>


namespace {

static const char alphanum[] =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";

static const size_t max_index = (sizeof(alphanum) - 1);

} // namespace unnamed

namespace hstrings {

char rand_char()
{
    return alphanum[rand() % (max_index - 1)];
}

std::string &rand_str(std::string &str, size_t lenth)
{
    str.resize(lenth);
    std::generate_n(&str[0], lenth, rand_char);
    return str;
}

std::string rand_str(size_t lenth)
{
    std::string res;
    return rand_str(res, lenth);
}

bool is_digit_only(const std::string &s)
{
    return !s.empty() && std::find_if(s.begin(),
        s.end(), [](char c) { return !std::isdigit(c); }) == s.end();
}

void split(const std::string &str, std::vector<std::string> &res, char delim)
{
    std::string::size_type i = 0;
    std::string::size_type j = str.find(delim);

    while (j != std::string::npos) {
        res.push_back(str.substr(i, j-i));
        i = ++j;
        j = str.find(delim, j);
    }
    res.push_back(str.substr(i, str.length()));
}

std::vector<std::string> splited(const std::string &str, char delim)
{
    std::vector<std::string> res;
    split(str, res, delim);
    return res;
}




} // namespase hstrings
