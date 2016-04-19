#include <random>
#include <iostream>
#include <functional>
#include <memory>
#include <map>
#include <algorithm>
#include <string>
#include <iostream>
#include <thread>
#include <set>

#include "Logger/Logger.hpp"

int main () {

    std::set<int> s = { 1, 2, 3, 4, 5 };
    std::vector<int> v;

    // multiply set's elements by 100 and store it into v
    std::transform(s.begin(), s.end(),    // source
        std::back_inserter(v),    // destination
        std::bind(std::multiplies<int>(), std::placeholders::_1, 100)  // C++11
        );
    for (auto & r : v)
        log_main("%", r);

}

