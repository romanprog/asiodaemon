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

size_t HdfsBuffer::accepted_content_size()
{
    return top_offset() - unparsed_offset();
}

size_t HdfsBuffer::calculate_mem(size_t block_size)
{
    // size_t block_size = dns::max_pkg_size ? dns::max_pkg_size : 1;
    size_t reserve_bl_count {1};

    return ((top_offset() + size_filled()) / block_size + reserve_bl_count) * block_size;
}

void HdfsBuffer::when_have_new_part(const size_t begin_offset, const size_t size)
{
    // HTTP head.
    if (_http_resp_code < 0) {
        std::vector<std::string> http_head = hstrings::splitted(std::string(data() + begin_offset, size), ' ');
        if ((http_head.size() != 3) || (!hstrings::is_digit_only(http_head[1]))) {
            _http_error = true;
            return;
        }
        _http_ver = http_head[0];
        _http_resp_code = std::stoi(http_head[1]);
        _http_resp_str = http_head[2];
        return;
    }
    // Headers.
    std::vector<std::string> http_header = hstrings::splitted(std::string(data() + begin_offset, size), ':');
    if (http_header.size() != 2) {
        _http_error = true;
        return;
    }
    if (http_header[0] == "Content-Length") {
        if (!hstrings::is_digit_only(http_header[1])) {
            _http_error = true;
            return;
        }
        _content_length = std::stoull(http_header[1]);
    }
    _head_list.emplace(http_header[0], hstrings::trim(http_header[1]));

    // End of headers (empty line).
    if (!size) {
        if (_http_resp_code < 0) {
            _http_error = true;
            return;
        }
        parser_off();
        _waiting_for_data = true;
    }


}


} //namespace hdfs


