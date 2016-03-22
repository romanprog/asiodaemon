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


std::vector<std::string> split(const std::string &str, char delim)
{
    std::string str_buf;
    std::stringstream sstr(str);
    std::vector<std::string> res;

    while (std::getline(sstr, str_buf, delim))
            res.push_back(str_buf);

    return std::move(res);

}


} // namespase hstrings
