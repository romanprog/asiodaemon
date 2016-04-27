#ifndef QUERY_HPP
#define QUERY_HPP

#include <string>
#include <vector>
#include <cstring>

#include "RedisTypes.hpp"
#include "commands-traits.hpp"
#include "../../Logger/Logger.hpp"

namespace redis {

// Utils for query builder.
void ins_query_prefix(std::string & query_, unsigned pcount_);
void add_query_part(std::string & query_, const char * part_);

template <typename CmdType>
class  query
{
private:
    using QueryCallbackType = std::function<void (RedisError, typename CmdType::return_type &)>;
public:

    // Overload for queryes without arguments.
    template <typename T = CmdType,
              typename = std::enable_if_t<T::no_params &&
                                          !std::is_base_of<cmd::one_line, T>::value>
              >
    explicit query(QueryCallbackType cb)
        : _cb(cb)
    {
        // Build query. See description in overload below.
        _query.resize(query_pref_max_size);
        _build_RESP_next(CmdType::name);
        ins_query_prefix(_query, _pcount);

    }
    // Overload for queryes with arguments.
    template <typename T = CmdType,
              typename ...Args,
              typename = std::enable_if_t<!T::no_params &&
                                          (sizeof ...(Args) > 0)>
              >
    explicit query(QueryCallbackType cb, Args && ...args)
        : _cb(cb)

    {
        // Count of all parameters for redis query will be known only after _build_RESP_query() finished work.
        // For maximum performance and to avoid memory relocation - reserved additional memory for
        // array lenth in the begining of query string.
        // Maximum text length of redis list size (2^32 - 1) + length of "*\r\n".
        //  unsigned pref_max_size = max_redis_list_size() + 3;
        _query.resize(query_pref_max_size);
        // Debug filling.
        // std::fill(_query.begin(), _query.end(), '0');

        // Recurcive query building.
        _build_RESP_next(CmdType::name, std::forward<Args>(args)...);

        // Add prefix "*count\r\n" (see RESP protocol docs).
        ins_query_prefix(_query, _pcount);

    }

    // Overload for simple one-line query(like "incr test" or "set test 1") with string as parameter;
    template <typename T = CmdType,
              typename = std::enable_if_t<T::no_params &&
                                          std::is_base_of<cmd::one_line, T>::value>
              >
    explicit query(QueryCallbackType cb, const std::string & query_)
        : _cb(cb),
          _query(query_)

    {
        _query += "\r\n";
    }

    void print() {
        log_main(_query);
    }

private:

    QueryCallbackType _cb;
    unsigned _pcount{0};
    std::string _query;

    // Recursive query builder.
    template <typename ...Args>
    void _build_RESP_next(const std::string & part_, Args && ...args)
    {
        _build_RESP_next(part_.c_str());
        _build_RESP_next(std::forward<Args>(args)...);
    }

    inline void _build_RESP_next(const std::string & part_)
    {
        _build_RESP_next(part_.c_str());
    }

    template <typename ...Args>
    void _build_RESP_next(const char * part_, Args && ...args)
    {
        _build_RESP_next(part_);
        _build_RESP_next(std::forward<Args>(args)...);
    }

    inline void _build_RESP_next(const char * part_)
    {
        add_query_part(_query, part_);
        ++_pcount;
    }

    template <typename ...Args>
    void _build_RESP_next(const std::vector<std::string> & part_, Args && ...args)
    {
        _build_RESP_next(part_);
        _build_RESP_next(std::forward<Args>(args)...);
    }

    inline void _build_RESP_next(const std::vector<std::string> & part_)
    {
        for (auto & str : part_)
            _build_RESP_next(str.c_str());
    }

    template <typename ...Args>
    void _build_RESP_next(int part_, Args && ...args)
    {
        _build_RESP_next(part_);
        _build_RESP_next(std::forward<Args>(args)...);
    }

    inline void _build_RESP_next(int part_)
    {
        std::string tmp(std::to_string(part_));
        _build_RESP_next(tmp.c_str());
    }

};

} // namespace redis

#endif // QUERY_HPP
