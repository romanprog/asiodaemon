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

#include "AsyncEvent/Redis/query.hpp"
#include "AsyncEvent/Redis/buff_adapters.hpp"
#include "AsyncEvent/Redis/thread_safe.hpp"
#include "AsyncEvent/Redis/conn_pool.hpp"

/// ///////  Single query template overloads /////////////////
void simple_query(const std::string & query)
{

}

/// ///////  Pipeline queryes template overload /////////////////
template <typename T,
          typename = typename std::enable_if<!T::is_blocking>::type>
void pipeline_query_add(redis::query<T> qu)
{

}

int main () {

    std::shared_ptr<std::string> s {std::make_shared<std::string>("Shared string")};
    redis::threadsafe::conn_queue<std::shared_ptr<std::string>> _qu;
    std::shared_ptr<std::string> s1;

    std::queue<std::shared_ptr<std::string>> test_queue;
    for (int i = 0; i < 1000; ++i)
        test_queue.push(s);

    for (int i = 0; i < 10000000; ++i)
    {
        test_queue.push(std::move(test_queue.front()));
        test_queue.pop();
    }

    _qu.push(s);

    if (_qu.get_first_free(s1))
        log_main("have free:%", s1.use_count());

    s.reset();

    if (_qu.get_first_free(s1))
        log_main("have free:%", s1.use_count());

    exit(0);

    std::string q("FIRST");
    auto cb = [](int err, redis::RespData & res)
    {

    };
    using namespace redis::cmd;
    redis::buff::adapter<std::string> rstr(std::ref(q));

    // log_main("%", sizeof(rstr));

    std::vector<std::string> vlist {"123", "asdasd", "asdasdaq12", "sadfasf", "sadasdf", "dasdf", "asfasdfasd", "asd"};
    std::function<void(const std::string &)> fn = simple_query;
    //    for (int i = 0; i < 1000000; ++i)
    //log_main("%", sizeof(fn));
    redis::query<append, redis::buff::enable_direct_buff> tq1(cb, rstr, "12345");
    // tq1.print();



}
