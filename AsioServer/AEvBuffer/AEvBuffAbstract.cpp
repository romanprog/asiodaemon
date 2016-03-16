#include "AEvBuffAbstract.hpp"
#include <cstring>
#include <iostream>

namespace aev {

AEvBuffAbstract::AEvBuffAbstract(size_t block_size, unsigned rs_bl_cnt,  std::string separator)
    :_reserve_bl_count(rs_bl_cnt),
     _block_size(!block_size? 1 : block_size),  // Prevent division by zero.
     _separator(separator),
     _reserved(_reserve_bl_count * _block_size)
{
    // In this case will be called _mem_calc() of base class, as need to allocate first memory block.
    // Override _mem_calc() in derived classes to make own memory managment in release() method
    // and other memory managers of derived classes.
     _cdata = static_cast<char *>(malloc(calculate_mem()));
}

AEvBuffAbstract::~AEvBuffAbstract()
{
    // Free main data block.
    free(_cdata);
}

const char *AEvBuffAbstract::data() const
{
    return static_cast<const char * const> (_cdata);
}

void *AEvBuffAbstract::data_top()
{
    // No free space.
    if (!size_avail())
        return nullptr;

    // Pointer to begin of free space.
    return _cdata + _top_offset;
}

int AEvBuffAbstract::parse(size_t bytes_readed)
{
    _top_offset += bytes_readed;

    if (bytes_readed < _separator.length())
        return -1;

    const char * d_tmp = _cdata + _unparsed_offset;
    const char * const d_end =_cdata + _top_offset - _separator.length();
    int new_parts_count {0};

    while (d_tmp <= d_end)
    {
        if (!memcmp(d_tmp, _separator.c_str(), _separator.length())) {
            d_tmp += _separator.length();
            size_t part_size = d_tmp - _cdata - _unparsed_offset;
            _data_parts.push_back({_unparsed_offset, part_size});
            _unparsed_offset += part_size;
            ++new_parts_count;

        } else {
            ++d_tmp;
        }
    }
    return new_parts_count;
}

void AEvBuffAbstract::release(size_t size)
{

    if (size < size_avail())
        return;

    size_t _reserved_free = _reserved - _top_offset;

    if (_reserved_free<=size) {
        _reserved = calculate_mem();
        _cdata = static_cast<char *>(realloc(_cdata, _reserved));
    }
    _size = _top_offset + size;

}

size_t AEvBuffAbstract::size_filled() const
{
    return _top_offset;
}


void AEvBuffAbstract::reset()
{
    _reserved = calculate_mem();
    _cdata = static_cast<char *>(realloc(_cdata, _reserved));
    _top_offset = _unparsed_offset = _size = _part_cursor = 0;
    _data_parts.clear();
}

size_t AEvBuffAbstract::size() const
{
    return size_avail();
}

size_t AEvBuffAbstract::size_avail() const
{
    return _size - _top_offset;
}

size_t AEvBuffAbstract::redundant_data_size() const
{
    return _top_offset - _unparsed_offset;
}

std::string AEvBuffAbstract::get_separator() const
{
    return _separator;
}

const DataOffsetList &AEvBuffAbstract::get_offsets_list() const
{
    return _data_parts;
}

size_t AEvBuffAbstract::calculate_mem()
{
    // Base mem reserv calculate algorithm. 1 block for data needeng + 1 free block.
    return ((_top_offset+size_filled)/_block_size+_reserve_bl_count)*_block_size;
}


}
