#ifndef HDFSTYPES_HPP
#define HDFSTYPES_HPP
#include <string>

namespace hdfs {

enum class QueryType
{
    create_write,
    append,
    open_read,
    mkdir,
    rename,
    del,
    get_status,
    list,

};

struct RestQuery
{
    bool useSSL {false};
    std::string host;
    unsigned port {0};
    std::string path;
    std::string command;
};

} // namespace hdfs

#endif // HDFSTYPES_HPP
