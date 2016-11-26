#include "HdfsUtils.hpp"
#include "../../HUtils/HStrings.hpp"
#include "../../HUtils/HNet.hpp"

namespace hdfs {


bool utils::parse_url(const std::string url_, PUri & res_)
{
    // Check uri.
    std::vector<std::string> splitted_uri_ = hstrings::splitted(url_,':');
    if (splitted_uri_.size() != 3)
        return false;

    if (splitted_uri_[1][0] != '/' || splitted_uri_[1][1] != '/')
        return false;

    if (!hstrings::is_digit_only(splitted_uri_[2]))
        return false;

//    if (!hnet::is_fqdn(splitted_uri_[1]))
//        return false;

    res_.port = std::move(splitted_uri_[0]);
    res_.domain = std::move(splitted_uri_[1]);
    // trim "//" from begining of domain.
    res_.domain.erase(res_.domain.begin(), res_.domain.begin() + 2);
    res_.port = std::move(splitted_uri_[2]);
    return true;
}

} // namespace hdfs
