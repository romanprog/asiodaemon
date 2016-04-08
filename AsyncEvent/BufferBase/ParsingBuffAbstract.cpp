#include "ParsingBuffAbstract.hpp"

#include <cstring>
#include <algorithm>

ParsingBuffAbstract::ParsingBuffAbstract(std::string delimiter, size_t e_pt_sz)
    :_delimiter(delimiter),
     _expected_part_size(e_pt_sz)
{ }

int ParsingBuffAbstract::parse()
{

    const char * d_tmp = data() + _unsearched_offset;
    const char * const ds_end = data() + top_offset();
    int new_parts_count {0};

    while (true)
    {
        d_tmp = std::search(d_tmp, ds_end, _delimiter.begin(), _delimiter.end());
        if (d_tmp != ds_end) {
            d_tmp += _delimiter.length();
            size_t part_size = d_tmp - data() - _unparsed_offset;
            _data_parts.push_back({_unparsed_offset, part_size});
            _unparsed_offset += part_size;
            _unsearched_offset = _unparsed_offset;
            ++new_parts_count;
            when_have_new_part(_unparsed_offset-part_size, part_size);
        } else {
            // Search optimization. Prevents re-search.
            if (top_offset() - _unparsed_offset > _delimiter.size())
                _unsearched_offset = top_offset() - _delimiter.size();
            break;
        }

    }
    return new_parts_count;
}


size_t ParsingBuffAbstract::redundant_data_size() const
{
    return top_offset() - _unparsed_offset;
}

std::string ParsingBuffAbstract::get_delimiter() const
{
    return _delimiter;
}

void ParsingBuffAbstract::reset()
{
    BuffAbstract::reset();
    _unparsed_offset = _unsearched_offset = 0;
    _data_parts.clear();
}

void ParsingBuffAbstract::mem_reduce()
{
    if (!redundant_data_size()) {
        reset();
        return;
    }
    size_t free_tmp = size_avail();
    std::string redundant_tmp (data() + _unparsed_offset, redundant_data_size());
    reset();
    *this << redundant_tmp;
    release(free_tmp);

}

const DataOffsetList &ParsingBuffAbstract::get_offsets_list() const
{
    return _data_parts;
}

size_t ParsingBuffAbstract::calculate_mem()
{
    size_t reserve_bl_count {2};
    return ((top_offset() + size_filled()) / _expected_part_size + reserve_bl_count) * _expected_part_size;
}

void ParsingBuffAbstract::when_new_data_acc(size_t bytes_readed)
{
    parse();
}


BufferDataList get_buff_dala_list(const ParsingBuffAbstract &_buffer, bool trim_delimiter)
{
    BufferDataList _temp_data_list;

    for (auto pair_ : _buffer.get_offsets_list())
        _temp_data_list.push_back(std::string(_buffer.data() + pair_.first, (trim_delimiter ? pair_.second - _buffer.get_delimiter().length() : pair_.second)));

    return _temp_data_list;
}
