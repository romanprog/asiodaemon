#include <random>
#include <iostream>
#include <functional>
#include <memory>
#include <map>
#include <algorithm>
#include <string>
#include <iostream>
#include <thread>

#include "HUtils/HStrings.hpp"
#include "Logger/Logger.hpp"
#include "Config/GlobalConf.hpp"
#include "AsyncEvent/Redis/RedisBuffer.hpp"
#include "AsyncEvent/AEvBase/AEventAbstract.hpp"
#include "AsyncEvent/Redis/AEvRedisMod.hpp"


redis::RespData result;

asio::io_service main_io;

aev::AEvStrandPtr _ev_loop(std::make_shared<asio::strand>(main_io));

aev::AEvRedisMod redis_db(_ev_loop, "127.0.0.1", 6379);

int query_counter = 0;

void ra_handler(int err, const redis::RespData &result);

void rq_handler(int err, const redis::RespData & result)
{
    if (err)
        log_debug("Error num: %", err);
      ++query_counter;
    redis_db.async_query("get test", ra_handler);

}

void ra_handler(int err, const redis::RespData & result)
{
    if (err)
        log_debug("Error num: %", err);

    if (result.ires > 900000)
        exit(0);

}

int main () {

    Config::glob().read_config("main.conf");
    if (Config::glob().have_error()) {
        CLog::glob().write("Error loading config: %", Config::glob().error_text());
        exit(0);
    }
    Config::glob().set_opt("logging_level", "3");
    RedisBuffer buff;
    // log_main("%", sizeof(aev::AEventAbstract));
//    std::string result;

    if (!redis_db.connect())
        exit(1);

    redis_db.query_loop("incr test", [](int err, const redis::RespData & result){
        if (err || result.ires > 1000000)
            redis_db.stop();

    }, 1000000);

//    std::thread t ([&]() {main_io.run();});
//    std::thread t1 ([&]() {main_io.run();});
    main_io.run();
//    t.join();
//    t1.join();

}


