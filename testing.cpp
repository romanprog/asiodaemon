#include <random>
#include <iostream>
#include <functional>
#include <memory>
#include <map>
#include <algorithm>
#include <string>
#include <iostream>

#include "HUtils/HStrings.hpp"
#include "Logger/Logger.hpp"
#include "Config/GlobalConf.hpp"

int main () {
    Config config = Config::glob();
    config.read_config("main.conwf");
    if (config.have_error()) {
        CLog::glob().write("Error reading config. Error: %", config.error_text());
        exit(0);
    }

    std::string s("one:two:three:::");
    std::string result;
    if (hstrings::cut_part_reverse(s, result, ':', 0))
        std::cout << result << std::endl;
}

