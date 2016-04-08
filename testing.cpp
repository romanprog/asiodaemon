#include <random>
#include <iostream>
#include <functional>
#include <memory>
#include <map>
#include <algorithm>
#include <string>
#include <iostream>

#include "HUtils/HStrings.hpp"



int main () {

    std::string s("one:two:three:::");
    std::string result;
    if (hstrings::cut_part_reverse(s, result, ':', 0))
        std::cout << result << std::endl;
}

