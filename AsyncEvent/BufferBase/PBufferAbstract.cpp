#include "PBufferAbstract.hpp"
#include "../../Logger/Logger.hpp"

#include <cstring>
#include <algorithm>

PBufferAbstract::PBufferAbstract(std::string delimiter, size_t e_pt_sz)
    :_delimiter(std::move(delimiter)),
     _expected_part_size(e_pt_sz)
{ }

int PBufferAbstract::parse()
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
            if (!_enable_parsing)
                break;
        } else {
            // Search optimization. Prevents re-search.
            if (top_offset() - _unparsed_offset > _delimiter.size())
                _unsearched_offset = top_offset() - _delimiter.size();
            break;
        }

    }
    return new_parts_count;
}


size_t PBufferAbstract::redundant_data_size() const
{
    return top_offset() - _unparsed_offset;
}

std::string PBufferAbstract::get_delimiter() const
{
    return _delimiter;
}

void PBufferAbstract::reset(bool soft_reset)
{
    // log_debug("PBufferAbstract reset");
    BuffAbstract::reset(soft_reset);
    _unparsed_offset = _unsearched_offset = 0;
    _data_parts.clear();
}

void PBufferAbstract::mem_reduce()
{
    size_t rd_data_size = redundant_data_size();
    if (!rd_data_size) {
        reset();
        return;
    }
    // Run memory transformation, only if actual data size less then 10% of buffer size.
    if ((rd_data_size * 100.00)/top_offset() > 10)
        return;
    // log_debug("Reduce memory!");

    size_t free_tmp = size_avail();
    std::string redundant_tmp (data() + _unparsed_offset, redundant_data_size());
    reset();
    *this << redundant_tmp;
    release(free_tmp);

}

const DataOffsetList &PBufferAbstract::get_offsets_list() const
{
    return _data_parts;
}

size_t PBufferAbstract::calculate_mem(size_t block_size)
{
    size_t reserve_bl_count {2};
    return ((top_offset() + size_filled()) / block_size + reserve_bl_count) * block_size;
}

void PBufferAbstract::when_new_data_acc(size_t bytes_readed)
{
    if (_enable_parsing)
        when_parsed_all(parse());
}


void PBufferAbstract::parser_off()
{
    _enable_parsing = false;
}

void PBufferAbstract::parser_on()
{
    _enable_parsing = true;
}


BufferDataList get_buff_dala_list(const PBufferAbstract &_buffer, bool trim_delimiter)
{
    BufferDataList _temp_data_list;

    for (auto pair_ : _buffer.get_offsets_list())
        _temp_data_list.push_back(std::string(_buffer.data() + pair_.first, (trim_delimiter ? pair_.second - _buffer.get_delimiter().length() : pair_.second)));

    return _temp_data_list;
}
