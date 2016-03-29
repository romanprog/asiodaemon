#include <random>
#include <iostream>
#include <functional>
#include <memory>

// friend class
#include <iostream>
#include "HUtils/HStrings.hpp"

int main () {
    std::string str("1  2 3 4");

    for (std::string & substr : hstrings::splitted(str, ' ', true) )
        std::cout << substr;

    return 0;
}
