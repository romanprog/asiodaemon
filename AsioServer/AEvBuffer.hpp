#ifndef AEVBUFFER_HPP
#define AEVBUFFER_HPP

#include <string>
#include <vector>

namespace aev {

using DataOffset = std::pair<size_t, size_t>;

class AEvBuffer
{
public:

//     AEvBuffer();
     explicit AEvBuffer(size_t block_size, std::string && separator);

    ~AEvBuffer();

    const char * data() const;

    void * data_top();

    int parse(size_t bytes_readed);

    void release(size_t size);

    std::string get();

    size_t size() const;

    size_t reserved() const;

    void reset();

    size_t free_sz() const;

    void read_string_test(const std::string && str);

private:
    size_t _blocks_reserved {1};
    size_t _block_size {990};
    std::string _separator {' '};

    size_t _reserved;
    size_t _size {0};
    size_t _unparsed_offset{0};
    size_t _top_offset {0};

    std::vector<DataOffset> _data_parts;
    size_t _part_cursor {0};

    char * _cdata;

};

}
#endif // AEVBUFFER_HPP
