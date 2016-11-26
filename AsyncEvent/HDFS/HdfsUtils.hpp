#ifndef HDFSUTILS_HPP
#define HDFSUTILS_HPP
#include "HdfsTypes.hpp"

namespace hdfs {

class utils
{
public:
    static bool parse_url(const std::string url_, PUri &res_);
private:
    utils() = default;
};

} // namespace hdfs

#endif // HDFSUTILS_HPP
