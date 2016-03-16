#include "AEvBuffer.hpp"
#include <cstring>
#include <iostream>
namespace aev {

AEvBuffer::AEvBuffer(size_t block_size, std::string separator)
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

size_t AEvBuffer::redundant_data_size() const
{
    return _top_offset - _unparsed_offset;
}

std::string AEvBuffer::get_separator() const
{
    return _separator;
}

const DataOffsetList &AEvBuffer::get_offsets_list() const
{
    return _data_parts;
}


BufferDataList get_buff_dala_list(const AEvBuffer &_buffer, bool trim_separator)
{
    BufferDataList _temp_data_list;

    for (auto pair_ : _buffer.get_offsets_list())
        _temp_data_list.push_back(std::string(_buffer.data() + pair_.first, (trim_separator ? pair_.second - _buffer.get_separator().length() : pair_.second)));

    return std::move(_temp_data_list);
}



}
