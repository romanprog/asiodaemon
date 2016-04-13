#ifndef REDISTYPES_HPP
#define REDISTYPES_HPP

#include <string>
#include <vector>
#include <memory>

namespace redis {

enum class RespType
{
    simple_str,
    error_str,
    integer,
    bulk_str,
    array,
    empty
};
struct RespData
{
    RespType type {RespType::empty};
    std::string sres;
    int ires {0};
    std::vector<RespData> ares;
};

using RespDataPtr = std::unique_ptr<RespData>;

}

#endif // REDISTYPES_HPP
