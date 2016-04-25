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

struct def_query_opt
{
    static constexpr bool is_blocking {false};
    static constexpr bool enable_direct_send_buff {false};
    static constexpr bool enable_direct_recive_buff {false};
    static constexpr bool no_params {false};
    using return_type = redis::RespData;
};

struct set : public def_query_opt
{
    static constexpr auto name {"set"};
};

struct get : public def_query_opt
{
    static constexpr auto name {"get"};

};

struct blpop : public def_query_opt
{
    static constexpr bool is_blocking {true};
    static constexpr auto name {"blpop"};

};

struct one_line : public def_query_opt
{
    // Query is blocked connection.
    static constexpr bool is_blocking {false};
    static constexpr bool enable_direct_recive_buff {true};
    static constexpr bool no_params {true};
};

using RedisError = int;

template <typename qT>
class rquery
{
public:
    using cbType = std::function<void (RedisError, typename qT::return_type &)>;
    // Overload for queryes without arguments.
    template <typename T = qT,
              typename = std::enable_if_t<T::no_params &&
                                          !std::is_base_of<one_line, T>::value>
              >
    explicit rquery(cbType cb)
        : _cb(cb),
          _query("*1\r\n")
    {
        _build_RESP_query(qT::name);
    }
    // Overload for queryes with arguments.
    template <typename T = qT,
              typename ...Args,
              typename = std::enable_if_t<!T::no_params &&
                                          (sizeof ...(Args) > 0)>
              >
    explicit rquery(cbType cb, Args && ...args)
        : _cb(cb)

    {
        _query.resize(10);
        _build_RESP_query(qT::name, std::forward<Args>(args)...);
        std::string qpref("*");
        qpref.append(std::to_string(_pcount));
        qpref.append("\r\n");
        memcpy(&_query[10 - qpref.size()], qpref.data(), qpref.size());
        _query.erase(_query.begin(), _query.begin() + 10 - qpref.size());

    }

    // Overload for simple one-line query(like "incr test" or "set test 1");
    template <typename T = qT,
              typename = std::enable_if_t<T::no_params &&
                                          std::is_base_of<one_line, T>::value>
              >
    explicit rquery(cbType cb, const std::string & query_)
        : _cb(cb),
          _query(query_)

    {
        _query.append("\r\n");

    }
    void print() {
        log_main(_query);
    }

private:
    cbType _cb;
    unsigned _pcount{0};
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
        _query.append("\r\n");
        _query.append(part_);
        _query.append("\r\n");
        ++_pcount;
    }

    template <typename ...Args>
    void _build_RESP_query(std::vector<std::string> part_, Args && ...args)
    {
        _build_RESP_query(part_);
        _build_RESP_query(std::forward<Args>(args)...);
    }

    inline void _build_RESP_query(std::vector<std::string> part_)
    {
        for (auto & str : part_)
            _build_RESP_query(str);
    }

    template <typename ...Args>
    void _build_RESP_query(int part_, Args && ...args)
    {
        _build_RESP_query(part_);
        _build_RESP_query(std::forward<Args>(args)...);
    }

    inline void _build_RESP_query(int part_)
    {
        _build_RESP_query(std::to_string(part_));
    }

};

/// ///////  Single query template overloads /////////////////
void simple_query(const std::string & query)
{

}

/// ///////  Pipeline queryes template overload /////////////////
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
    std::vector<std::string> vlist {"123", "asdasd", "asdasdaq12", "sadfasf", "sadasdf", "dasdf", "asfasdfasd", "asd"};
    rquery<set> tq1(cb, "key", vlist, 100);
    tq1.print();
    pipeline_query_add(tq1);
    q = "   123345";
    log_main("%", q.capacity());
    q.erase(q.begin(), q.begin() + 3);
    log_main(q);
    log_main("%", q.capacity());


}
