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

struct set
{
    // Query is blocked connection.
    static constexpr bool is_blocking {false};
    static constexpr bool enable_direct_send_buff {true};
    static constexpr bool enable_direct_recive_buff {false};
    static constexpr bool no_params {false};
    static constexpr auto text {"set\r\n"};
    using return_type = redis::RespData;

};

struct get
{
    // Query is blocked connection.
    static constexpr bool is_blocking {false};
    static constexpr bool enable_direct_send_buff {false};
    static constexpr bool enable_direct_recive_buff {true};
    static constexpr bool no_params {true};
    static constexpr auto text {"get\r\n"};
    using return_type = redis::RespData;

};

struct blpop
{
    // Query is blocked connection.
    static constexpr bool is_blocking {false};
    static constexpr bool enable_direct_send_buff {false};
    static constexpr bool enable_direct_recive_buff {true};
    static constexpr bool no_params {false};
    static constexpr auto text {"blpop\r\n"};
    typedef redis::RespData return_type;

};

struct one_line
{
    // Query is blocked connection.
    static constexpr bool is_blocking {false};
    static constexpr bool enable_direct_send_buff {false};
    static constexpr bool enable_direct_recive_buff {true};
    static constexpr bool no_params {false};
    static constexpr auto text {"blpop\r\n"};
    typedef redis::RespData return_type;

};

using RedisError = int;

template <typename qT>
class rquery
{
public:
    using cbType = std::function<void (RedisError, typename qT::return_type &)>;
    template <typename T = qT,
              typename = std::enable_if_t<T::no_params>
              >
    explicit rquery(cbType cb)
        : _cb(cb),
          _args_count(0),
          _query("*1\r\n")
    {
        _query += qT::text;
    }
    // Overload for queryes with arguments.
    template <typename T = qT,
              typename ...Args,
              typename = std::enable_if_t<!T::no_params &&
                                          !std::is_base_of<one_line, T>::value
                                          >
              >
    explicit rquery(cbType cb, Args && ...args)
        : _cb(cb),
          _args_count(sizeof ...(Args))

    {
        _query = "*";
        _query += std::to_string(sizeof ...(Args));
        _query += "\r\n";
        _build_RESP_query(std::forward<Args>(args)...);

    }

    // Overload for simple one-line query(like "incr test" or "set test 1");
    template <typename T = qT,
              typename = std::enable_if_t<!T::no_params &&
                                          std::is_base_of<one_line, T>::value
                                          >
              >
    explicit rquery(cbType cb, const std::string & query_)
        : _cb(cb),
          _args_count(1),
          _query(query_)

    {
        _query += "\r\n";

    }
    void print() {
        log_main(_query);
    }

private:
    cbType _cb;
    const int _args_count;
    std::string _query;

    // Recursive query builder.
    template <typename ...Args>
    void _build_RESP_query(const std::string & part_, Args && ...args)
    {
        _build_RESP_query(part_);
        _build_RESP_query(std::forward<Args>(args)...);
    }

    inline void _build_RESP_query(const std::string & part_)
    {
        _query.append("$");
        _query += std::to_string(part_.size());
        _query.append(part_);
        _query.append("\r\n");
    }

};

/// ///////  Single query template overloads /////////////////
void simple_query(const std::string & query)
{

}

/// ///////  Pipeline queryes template overloads /////////////////
template <typename T,
          typename = typename std::enable_if<!T::is_blocking>::type>
void pipeline_query_add(rquery<T> qu)
{

}

int main () {

    std::string q;
    auto cb = [](int err, redis::RespData & res)
    {

    };

    rquery<one_line> tq1(cb, "set test 1");
    tq1.print();
    pipeline_query_add(tq1);


}
