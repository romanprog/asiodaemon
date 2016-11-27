#ifndef HDFSUTILS_HPP
#define HDFSUTILS_HPP
#include "HdfsTypes.hpp"

namespace hdfs {

class utils
{
public:
    static bool parse_url(const std::string &url_, Endpoint &res_);
    static bool parse_location(const std::string &lstring_, Location &res_);
private:
    utils() = default;
};

} // namespace hdfs

#endif // HDFSUTILS_HPP
