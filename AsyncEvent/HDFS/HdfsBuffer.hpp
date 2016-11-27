#ifndef HDFSBUFFER_HPP
#define HDFSBUFFER_HPP

#include "../BufferBase/PBufferAbstract.hpp"

#include <string>
#include <vector>
#include <unordered_map>

namespace hdfs {
using HeadersListMap = std::unordered_map<std::string, std::string>;

class HdfsBuffer : public PBufferAbstract
{
public:
    HdfsBuffer();

    void clear();

    inline bool have_error()
    {
        return _http_error;
    }
    const std::string &h_get(const std::string & hname_);

    bool h_is_exist(const std::string & hname_);

    inline int get_resp_code()
    {
        return _http_resp_code;
    }

    inline const char * const content() const
    {
        return data() + (top_offset() - _content_length);
    }

    inline size_t content_size() const
    {
        return _content_length;
    }

    std::string get_redirect_location();

    size_t unaccepted_content_size();

protected:
    virtual size_t calculate_mem(size_t block_size) override;
    virtual void when_have_new_part(const size_t begin_offset, const size_t size) override;

private:
    bool redy_for_respond {false};

    // List of http headers.
    HeadersListMap _head_list;

    // Sample: "HTTP/1.1 307 TEMPORARY_REDIRECT":
    std::string _http_ver; // "HTTP/1.1"
    int _http_resp_code {-1}; // 307
    std::string _http_resp_str; // "TEMPORARY_REDIRECT"
    size_t _content_length {0};

    // All headers received. Waiting for data (_content_length).
    bool _waiting_for_data {false};

    // Protocol error in respond.
    bool _http_error {false};


};

} //namespace hdfs

#endif // HDFSBUFFER_HPP
