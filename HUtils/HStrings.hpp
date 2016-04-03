#ifndef HSTRINGS_HPP
#define HSTRINGS_HPP

#include <string>
#include <vector>

namespace hstrings {

// Generate random char from alphanum[] set.
inline char rand_char();

// Very fast random string generator. Fill str by random chars. Perform resize of str. Return str ref.
std::string & rand_str(std::string & str, size_t lenth);
std::string rand_str(size_t lenth);

bool is_digit_only(const std::string & s);
bool is_alfa_only(const std::string & s);

std::vector<std::string> splitted(const std::string & str, char delim, bool remove_empty = false);
std::vector<std::string> & split(const std::string & str, std::vector<std::string> &res, char delim, bool remove_empty = false);

// Convert string to lower case.
std::string & to_lower(std::string & text);

// Get part number @pindex of string separated by @delim.
// Numbering is zero base.
bool get_part(const std::string & text, std::string &result, char delim, size_t pindex = 0);


bool cut_part(const std::string & text, std::string &result, char delim, size_t pindex = 0);

std::string & trim_l(std::string & text);
std::string & trim_r(std::string & text);
std::string & trim(std::string & text);


}
#endif // HSTRINGS_HPP
