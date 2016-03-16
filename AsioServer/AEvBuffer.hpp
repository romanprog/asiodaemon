#ifndef AEVBUFFER_HPP
#define AEVBUFFER_HPP

#include <string>
#include <vector>

namespace aev {

using DataOffset = std::pair<size_t, size_t>;
using DataOffsetList = std::vector<DataOffset>;
using BufferDataList = std::vector<std::string>;

class AEvBuffer
{
public:
    explicit AEvBuffer(size_t block_size, std::string separator);
    ~AEvBuffer();

    const char * data() const;
    void * data_top();
    int parse(size_t bytes_readed);
    void release(size_t size);

    void reset();

    size_t size() const;
    size_t reserved() const;
    size_t free_sz() const;
    size_t redundant_data_size() const;
    std::string get_separator() const;

    const DataOffsetList & get_offsets_list() const;

    // temporary for test
    void read_string_test(const std::string && str);
    std::string get();

private:

    size_t _blocks_reserved {1};
    size_t _block_size {4096};
    std::string _separator;

    size_t _reserved;
    size_t _size {0};
    size_t _unparsed_offset{0};
    size_t _top_offset {0};

    DataOffsetList _data_parts;
    size_t _part_cursor {0};


    char * _cdata;

};

    BufferDataList get_buff_dala_list(const AEvBuffer & _buffer, bool trim_separator = true);

}
#endif // AEVBUFFER_HPP
