#ifndef PARSINGBUFFABSTRACT_HPP
#define PARSINGBUFFABSTRACT_HPP

#include "BuffAbstract.hpp"

#include <string>
#include <vector>

using DataOffset = std::pair<size_t, size_t>;
using DataOffsetList = std::vector<DataOffset>;
using BufferDataList = std::vector<std::string>;


class ParsingBuffAbstract : public BuffAbstract

{
public:
    ParsingBuffAbstract(std::string delimiter, size_t e_pt_sz);

    // Return vector of parsed parts in buffer. Format:
    // std::pair
    // 1) first byte offset from the beginning of the buffer;
    // 2) part size (including delimiter);
    const DataOffsetList & get_offsets_list() const;

    // Return bytes count, wich not have separator at end (after parsing).
    // This bytes will be parsed next time. New data will be added at end.
    size_t redundant_data_size() const;
    // Return separator.
    std::string get_separator() const;

    // Full buffer reset. One more override not allowed. Use virtual method when_reseted().
    void reset() override final;

    // Free mem of parsed data. Reset bufer and write in it unparsed part.
    void mem_reduce();

protected:
    virtual size_t calculate_mem() override;
    virtual void when_new_data_acc(size_t bytes_readed) override;
    virtual void when_have_new_part(const size_t begin_offset, const size_t size) = 0;

private:

    int parse();
    const std::string _delimiter;
    const size_t _expected_part_size;
    size_t _unparsed_offset{0};
    DataOffsetList _data_parts;

};

BufferDataList get_buff_dala_list(const ParsingBuffAbstract &_buffer, bool trim_separator = true);

#endif // PARSINGBUFFABSTRACT_HPP
