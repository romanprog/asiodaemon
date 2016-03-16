#ifndef AEVBUFFER_HPP
#define AEVBUFFER_HPP

#include <string>
#include <vector>

namespace aev {

using DataOffset = std::pair<size_t, size_t>;
using DataOffsetList = std::vector<DataOffset>;
using BufferDataList = std::vector<std::string>;

class AEvBuffAbstract
{
public:
    explicit AEvBuffAbstract(size_t block_size, unsigned rs_bl_cnt,  std::string separator);
    virtual ~AEvBuffAbstract();

    // Return pointer to first byte of read-only buffer data.
    const char * data() const;

    // Return pointer to first byte of free space.
    void * data_top();
    int parse(size_t bytes_readed);
    void release(size_t size_filled);

    // Reset buffer, clear data, and ser all counters to default.
    void reset();

    // In base variant - return size_avail(). Can be overridden in derived classes.
    virtual size_t size() const;

    // Return actual data size, stored in buffer.
    size_t size_filled() const;

    // Return size of free buffer space, allowing to store new data.
    size_t size_avail() const;


    const DataOffsetList & get_offsets_list() const;

protected:
    virtual size_t calculate_mem();
    virtual void when_have_new_part(const std::string line) = 0;
    virtual void when_parsed(unsigned new_parts_count) = 0;
    virtual void when_reseted() = 0;

    // Return bytes count, wich not have separator at end (after parsing).
    // This bytes will be parsed next time. New data will be added at end.
    size_t redundant_data_size() const;
    // Return separator.
    std::string get_separator() const;

private:

    const unsigned _reserve_bl_count;
    const size_t _block_size;
    const std::string _separator;

    size_t _reserved;
    size_t _size {0};
    size_t _unparsed_offset{0};
    size_t _top_offset {0};

    DataOffsetList _data_parts;
    size_t _part_cursor {0};

    char * _cdata;

};

}
#endif // AEVBUFFER_HPP
