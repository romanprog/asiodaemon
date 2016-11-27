#include "HdfsUtils.hpp"
#include "../../HUtils/HStrings.hpp"
#include "../../HUtils/HNet.hpp"

namespace hdfs {


bool utils::parse_url(const std::string & url_, Endpoint & res_)
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

bool utils::parse_location(const std::string & lstring_, Location &res_)
{
    size_t pos {0};
    for (int i = 0; i < 3 && pos != std::string::npos; ++i) {
        pos = lstring_.find('/', pos+1);
        auto dbg = 1;
        dbg = pos;
    }
    if (pos == std::string::npos)
        return false;

    std::string ep_url_part {lstring_.cbegin(), lstring_.cbegin() + pos};

    if (!utils::parse_url(ep_url_part, res_.endpoint))
        return false;

    res_.query.assign(lstring_.cbegin() + pos, lstring_.cend());
    return true;

}

} // namespace hdfs
