#ifndef COMMANDSTRAITS_HPP
#define COMMANDSTRAITS_HPP

#include "RedisTypes.hpp"

namespace redis {
namespace cmd {

//namespace {
// Base set of command trains and properties.
struct default_traits
{
    static constexpr bool is_blocking {false};
    static constexpr bool enable_direct_send_buff {false};
    static constexpr bool enable_direct_recive_buff {false};
    static constexpr bool no_params {false};
    using return_type = RespData;
};
//} // namespace unnamed

struct one_line : public default_traits
{
    static constexpr bool is_blocking {false};
    static constexpr bool enable_direct_recive_buff {true};
    static constexpr bool no_params {true};
};


struct set : public default_traits
{
    static constexpr auto name {"set"};
};

struct get : public default_traits
{
    static constexpr auto name {"get"};
};

struct blpop : public default_traits
{
    static constexpr bool is_blocking {true};
    static constexpr auto name {"blpop"};
};


} // namespace cmd
} // namespace redis


#endif // COMMANDSTRAITS_HPP
