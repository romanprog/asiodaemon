#include "AEvBuffer.hpp"
#include <cstring>
#include <iostream>
namespace aev {
//AEvBuffer::AEvBuffer()
//   :_size(0)
//{
//    _reserved = (_size/_block_size+_blocks_reserved)*_block_size;
//    _cdata =  static_cast<char *>(malloc(_reserved));
//}

AEvBuffer::AEvBuffer(size_t block_size, std::string &&separator)
    :_block_size(block_size),
     _separator(separator),
     _reserved(_blocks_reserved * _block_size)
{
    _cdata = static_cast<char *>(malloc(_reserved));
}

AEvBuffer::~AEvBuffer()
{
    free(_cdata);
}

const char *AEvBuffer::data() const
{
    return static_cast<const char * const> (_cdata);
}

void *AEvBuffer::data_top()
{
    return _cdata + _top_offset;
}

int AEvBuffer::parse(size_t bytes_readed)
{
    _top_offset += bytes_readed;

    if (bytes_readed < _separator.length())
        return 0;

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

void AEvBuffer::release(size_t size)
{

    if (size < free_sz())
        return;

    size_t _reserved_free = _reserved - _top_offset;
    if (_reserved_free<=size) {
        _reserved = ((_top_offset+size)/_block_size+_blocks_reserved)*_block_size;
        _cdata = static_cast<char *>(realloc(_cdata, _reserved));
    }
    _size = _top_offset + size;

}

std::string AEvBuffer::get()
{
    if (_part_cursor >= _data_parts.size())
        return "";

    ++_part_cursor;
    return std::string(_cdata+_data_parts[_part_cursor-1].first, _data_parts[_part_cursor-1].second);
}

size_t AEvBuffer::size() const
{
    return _size;
}

size_t AEvBuffer::reserved() const
{
    return _reserved;
}

void AEvBuffer::reset()
{
    _reserved = _blocks_reserved * _block_size;
    _cdata = static_cast<char *>(realloc(_cdata, _reserved));
    _top_offset = _unparsed_offset = _size = _part_cursor = 0;
    _data_parts.clear();
}

size_t AEvBuffer::free_sz() const
{
    return _size - _top_offset;
}

void AEvBuffer::read_string_test(const std::string &&str)
{
    release(str.size());
    memcpy(_cdata + _top_offset, str.c_str(), str.size());
    parse(str.size());
}

}
