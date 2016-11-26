#include "HdfsBuffer.hpp"
#include "../../HUtils/HStrings.hpp"

#include <cstring>
#include <iostream>

namespace hdfs {

HdfsBuffer::HdfsBuffer()
    :PBufferAbstract::PBufferAbstract("\r\n", 2048)
{ }

void HdfsBuffer::clear()
{
    reset();
}

const std::string & HdfsBuffer::h_get(const std::string &hname_)
{
    return _head_list.at(hname_);
}

bool HdfsBuffer::h_is_exist(const std::string &hname_)
{
    try {
        _head_list.at(hname_);
    }
    catch (std::out_of_range& oor)
    {
        return false;
    }
    return true;
}

std::string HdfsBuffer::get_redirect_location()
{
    std::string res;
    if (h_is_exist("Location"))
        res = _head_list.at("Location");

    return res;
}

size_t HdfsBuffer::unaccepted_content_size()
{
    return top_offset() - unparsed_offset() - _content_length;
}

size_t HdfsBuffer::calculate_mem(size_t block_size)
{
    // size_t block_size = dns::max_pkg_size ? dns::max_pkg_size : 1;
    size_t reserve_bl_count {1};

    return ((top_offset() + size_filled()) / block_size + reserve_bl_count) * block_size;
}

void HdfsBuffer::when_have_new_part(const size_t begin_offset, const size_t size)
{
    // End of headers (empty line).
    if (size == 2) {
        if (_http_resp_code < 0) {
            _http_error = true;
            return;
        }
        parser_off();
        _waiting_for_data = true;
    }

    std::string line_tmp {data() + begin_offset, size};
    line_tmp.resize(line_tmp.size()-2);

    // HTTP head.
    if (_http_resp_code < 0) {
        auto res = hstrings::get_part(line_tmp,_http_ver,' ');
        std::string code_tmp;
        res &= hstrings::get_part(line_tmp, code_tmp,' ', 1);

        if (!res || !hstrings::is_digit_only(code_tmp)) {
            _http_error = true;
            return;
        }
        _http_resp_code = std::stoi(code_tmp);

        _http_resp_str = std::move(line_tmp);
        _http_resp_str.erase(_http_resp_str.begin(), _http_resp_str.begin() + code_tmp.size() + _http_ver.size() + 2);
        return;
    }

    // Headers.
    std::string hname;

    auto res = hstrings::get_part(line_tmp, hname, ':');
    if (!res) {
        _http_error = true;
        return;
    }
    line_tmp.erase(line_tmp.begin(), line_tmp.begin() + hname.size() + 2);

    if (hname == "Content-Length") {
        if (!hstrings::is_digit_only(line_tmp)) {
            _http_error = true;
            return;
        }
        _content_length = std::stoull(line_tmp);
    }
    _head_list.emplace(std::move(hname), std::move(line_tmp));

}


} //namespace hdfs


