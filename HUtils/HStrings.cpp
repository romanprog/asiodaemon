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

bool is_alfa_only(const std::string &s)
{
    return !s.empty() && std::find_if(s.begin(),
        s.end(), [](char c) { return !std::isalpha(c); }) == s.end();
}


std::vector<std::string> &split(const std::string &str, std::vector<std::string> &res, char delim, bool remove_empty)
{
    std::string::size_type j{0}, i{0};

    while (true) {
        j = str.find(delim, j);

        if (!remove_empty || i != j)
            res.push_back(str.substr(i, j-i));

        if (j == std::string::npos)
            break;

        i = ++j;

    }
    return res;
}

std::vector<std::string> splitted(const std::string &str, char delim, bool remove_empty)
{
    std::vector<std::string> res;
    split(str, res, delim, remove_empty);
    return res;
}

std::string &to_lower(std::string &text)
{
    std::transform(text.begin(), text.end(), text.begin(), ::tolower);
    return text;
}





} // namespase hstrings
