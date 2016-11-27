#ifndef HDFSTYPES_HPP
#define HDFSTYPES_HPP
#include <string>
#include <functional>


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

struct Target
{
    Target(std::string name_node_uri_, std::string fname_, std::string username_)
        : name_node_uri(std::move(name_node_uri_)),
          filename(std::move(fname_)),
          username(std::move(username_))
    {}
    Target(Target && other) noexcept
        : name_node_uri(std::move(other.name_node_uri)),
          filename(std::move(other.filename)),
          username(std::move(other.username))
    {}
    Target & operator=(Target && other) noexcept
    {
        name_node_uri = std::move(other.name_node_uri);
        filename = std::move(other.filename);
        username = std::move(other.username);
        return *this;
    }

    std::string name_node_uri;
    std::string filename;
    std::string username;
};

struct Endpoint
{
    std::string proto;
    std::string domain;
    std::string port;
};

struct Location
{
    Endpoint endpoint;
    std::string query;
};


} // namespace hdfs

#endif // HDFSTYPES_HPP
