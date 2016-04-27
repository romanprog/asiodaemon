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

    std::string q("10asdfkjasldkjfhlakjsdhfkjashdfhasdljfhakljsdhvlkjcxvkjadskjhvjhadsgvjhabsdvhbadshbvhjasbdvhbashvb00");
    auto cb = [](int err, redis::RespData & res)
    {

    };
    std::vector<std::string> vlist {"123", "asdasd", "asdasdaq12", "sadfasf", "sadasdf", "dasdf", "asfasdfasd", "asd"};
    for (int i = 0; i < 1000000; ++i)
         redis::query<redis::cmd::one_line> tq1(cb, "inct test 100");
    // tq1.print();
//    pipeline_query_add(tq1);
//    q = "   123345";
//    log_main("%", q.capacity());
//    q.erase(q.begin(), q.begin() + 3);
//    log_main(q);
//    log_main("%", q.capacity());
//    log_main("%", max_redis_list_size());


}
