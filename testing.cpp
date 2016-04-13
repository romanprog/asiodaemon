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
#include "AsyncEvent/Redis/RedisBuffer.hpp"

int main () {
    Config config = Config::glob();
    config.read_config("main.conf");
    if (config.have_error()) {
        CLog::glob().write("Error loading config: %", config.error_text());
        exit(0);
    }
    Config::glob().set_opt("logging_level", "3");
    RedisBuffer buff;
    for (int i = 0; i < 1000000; ++i)
    {

        buff << "$20\r\na2201oajqkeiskaumsya\r\n";

        if (buff.is_complate())
            buff.reset(true);
    }

}

