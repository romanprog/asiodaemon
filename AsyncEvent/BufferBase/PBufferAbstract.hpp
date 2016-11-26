#ifndef PARSINGBUFFABSTRACT_HPP
#define PARSINGBUFFABSTRACT_HPP

#include "BuffAbstract.hpp"

#include <string>
#include <vector>

using DataOffset = std::pair<size_t, size_t>;
using DataOffsetList = std::vector<DataOffset>;
using BufferDataList = std::vector<std::string>;


class PBufferAbstract : public BuffAbstract

{
public:
    PBufferAbstract(std::string delimiter, size_t e_pt_sz);

    // Return reference to vector of parsed parts in buffer. Format:
    // std::pair
    // 1) offset from the buffer beginning ;
    // 2) part size (including delimiter);
    const DataOffsetList & get_offsets_list() const;

    // Return bytes count, wich not have delimiter at end (after parsing).
    // This bytes will be parsed next time. New data will be added at end.
    size_t redundant_data_size() const;

    // Return delimiter.
    std::string get_delimiter() const;

    // Full buffer reset. One more override not allowed. Use virtual method when_reseted().
    void reset(bool soft_reset = false);

    // Free mem of parsed data. Reset bufer and write in it unparsed part.
    void mem_reduce();

    virtual ~PBufferAbstract() {}

protected:
    virtual size_t calculate_mem(size_t block_size) override;
    virtual void when_new_data_acc(size_t bytes_readed) override;
    virtual void when_have_new_part(const size_t begin_offset, const size_t size) = 0;
    virtual void when_parsed_all(size_t new_parts) {}

    inline size_t unparsed_offset()
    {
        return _unparsed_offset;
    }

    // On/off parsing.
    void parser_off();
    void parser_on();

private:

    int parse();
    const std::string _delimiter;
    const size_t _expected_part_size;
    size_t _unsearched_offset{0};
    size_t _unparsed_offset{0};
    bool _enable_parsing{true};
    DataOffsetList _data_parts;

};

BufferDataList get_buff_dala_list(const PBufferAbstract &_buffer, bool trim_delimiter = true);

#endif // PARSINGBUFFABSTRACT_HPP
