#include "ParsingBuffAbstract.hpp"

#include <cstring>
#include <iostream>

namespace aev {

ParsingBuffAbstract::ParsingBuffAbstract(std::string separator, size_t e_pt_sz)
    :_separator(separator),
     _expected_part_size(e_pt_sz)
{ }

int ParsingBuffAbstract::parse()
{

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
            when_have_new_part(_unparsed_offset-part_size, part_size);

        } else {
            ++d_tmp;
        }
    }
    return new_parts_count;
}


size_t ParsingBuffAbstract::redundant_data_size() const
{
    return _top_offset - _unparsed_offset;
}

std::string ParsingBuffAbstract::get_separator() const
{
    return _separator;
}

const DataOffsetList &ParsingBuffAbstract::get_offsets_list() const
{
    return _data_parts;
}

size_t ParsingBuffAbstract::calculate_mem()
{
    size_t reserve_bl_count {2};

    return ((_top_offset + size_filled()) / _expected_part_size + reserve_bl_count) * _expected_part_size;
}

void ParsingBuffAbstract::when_new_data_acc()
{
    parse();
}

void ParsingBuffAbstract::when_reseted()
{
    _unparsed_offset = _part_cursor = 0;
    _data_parts.clear();
}

BufferDataList get_buff_dala_list(const ParsingBuffAbstract &_buffer, bool trim_separator)
{
    BufferDataList _temp_data_list;

    for (auto pair_ : _buffer.get_offsets_list())
        _temp_data_list.push_back(std::string(_buffer.data() + pair_.first, (trim_separator ? pair_.second - _buffer.get_separator().length() : pair_.second)));

    return std::move(_temp_data_list);
}

}
