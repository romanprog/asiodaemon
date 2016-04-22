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
#include "Atomic/AtomicTypes.hpp"
#include "AsyncEvent/Redis/RedisBuffer.hpp"
#include "AsyncEvent/AEvBase/AEventAbstract.hpp"
#include "AsyncEvent/Redis/AEvRedisMod.hpp"
#include "AsyncEvent/Redis/RedisBuffer.hpp"

redis::RespData result;

asio::io_service main_io;

aev::AEvStrandPtr _ev_loop(std::make_shared<asio::strand>(main_io));

int respond_count_t1 = 0;
int respond_count_t2 = 0;
int respond_count_main = 0;


void f(std::string const & s)
{
    std::string k {s};
}

int main () {

    Config::glob().read_config("main.conf");
    if (Config::glob().have_error()) {
        CLog::glob().write("Error loading config: %", Config::glob().error_text());
        exit(0);
    }
    Config::glob().set_opt("logging_level", "3");

    aev::AEvRedisMod redis_db(_ev_loop);
    if (!redis_db.connect("127.0.0.1", 6379))
        exit(1);

    auto ra_handler = [&redis_db](int err, const redis::RespData & result)
    {
        if (err)
            log_debug("Error num: %", err);
        ++respond_count_main;
//        log_main("%", respond_count_main);
//        if (result.sres != "0")
//           log_main("error 0 %", result.sres);
        // log_main("%", result.ires);
    };




//    for (int i = 0; rbuff.parse_one(resp); ++i)
//        log_main("% %", i, resp.sres);


    redis_db.async_query("LRANGE mylist1 0 -1", ra_handler);
//    for (int i = 0; i < 1000000; ++i)
//    {
//        redis_db.async_query("LRANGE mylist1 0 -1", ra_handler);
//    }
    log_main("All sended.");

    redis_db.disconnect();
    log_main("Resived main = %, t1 = %, t2 = %", respond_count_main, respond_count_t1, respond_count_t2);
    //    t1.join();

    sleep(1);

}


