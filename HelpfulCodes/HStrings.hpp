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

std::vector<std::string> splited(const std::string & str, char delim);
void split(const std::string & str, std::vector<std::string> &res, char delim);

}
#endif // HSTRINGS_HPP
