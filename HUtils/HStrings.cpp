#include "HStrings.hpp"

#include <sstream>
#include <algorithm>
#include <utility>


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

std::string &trim_l(std::string &text)
{
    text.erase(text.begin(), std::find_if_not(text.begin(), text.end(), isspace));
    return text;
}

std::string &trim_r(std::string &text)
{
    text.erase(std::find_if_not(text.rbegin(), text.rend(), isspace).base() , text.end());
    return text;
}

std::string &trim(std::string &text)
{
    return trim_l(trim_r(text));
}

namespace  {

std::pair<size_t, size_t> pcoord(const std::string &text, char delim, size_t pindex)
{
   size_t pos_beg {0}, pos_end {0};

    for (size_t i = 0; i <= pindex; ++i)
    {
        pos_beg = text.find_first_not_of(delim, pos_end);
        if (pos_beg == std::string::npos)
            return {std::string::npos, std::string::npos};

        pos_end = text.find(delim, pos_beg);
    }
    if (pos_end == std::string::npos)
        pos_end = text.size();

    return {pos_beg, pos_end};
}

}

bool get_part(const std::string &text, std::string &result, char delim, size_t pindex)
{
    std::pair<size_t, size_t> substr_coords = pcoord(text, delim, pindex);
    if (substr_coords.first == std::string::npos)
        return false;

    result = text.substr(substr_coords.first, substr_coords.second - substr_coords.first);
    return true;
}

bool cut_part(const std::string & text, std::string &result, char delim, size_t pindex)
{
    std::pair<size_t, size_t> substr_coords = pcoord(text, delim, pindex);

    if (substr_coords.first == std::string::npos)
        return false;

    if (substr_coords.second < text.size())
        ++substr_coords.second;
    if (substr_coords.first > 0)
        -- substr_coords.first;

    result = text;
    result.erase(substr_coords.first, substr_coords.second - substr_coords.first);
    return true;
}

} // namespace hstrings