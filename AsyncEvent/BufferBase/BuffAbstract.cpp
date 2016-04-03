#include "BuffAbstract.hpp"

BuffAbstract::BuffAbstract()
    : _reserved(calculate_mem())
{
    // In this case will be called _mem_calc() of base class, as need to allocate first memory block.
    // Override _mem_calc() in derived classes to make own memory managment in release() method
    // and other memory managers of derived classes.
    _cdata = static_cast<char *>(malloc(_reserved));
}

BuffAbstract::~BuffAbstract()
{
    // Free main data block.
    free(_cdata);
}

const char *BuffAbstract::data() const
{
    return static_cast<const char * const> (_cdata);
}

void *BuffAbstract::data_top()
{
    // No free space.
    if (!size_avail())
        return nullptr;

    // Pointer to begin of free space.
    return _cdata + _top_offset;
}

void *BuffAbstract::vdata()
{
    return _cdata;
}

bool BuffAbstract::accept(size_t bytes_readed)
{
    if (bytes_readed > size_avail())
        return false;

    _top_offset += bytes_readed;

    when_new_data_acc(bytes_readed);
    return true;
}

void BuffAbstract::release(size_t size)
{

    if (size < size_avail())
        return;

    size_t _reserved_free = _reserved - _top_offset;

    if (_reserved_free <= size) {
        _reserved = calculate_mem();
        _cdata = static_cast<char *>(realloc(_cdata, _reserved));
    }
    _size = _top_offset + size;

}

size_t BuffAbstract::size_filled() const
{
    return _top_offset;
}


void BuffAbstract::reset()
{
    _top_offset = _size = 0;
    _reserved = calculate_mem();
    _cdata = static_cast<char *>(realloc(_cdata, _reserved));
    when_reseted();
}

size_t BuffAbstract::size() const
{
    return _size;
}

size_t BuffAbstract::size_avail() const
{
    return _size - _top_offset;
}

void BuffAbstract::operator <<(const std::string &str)
{
    release(str.size());
    memcpy(data_top(), str.data(), str.size());
    accept(str.size());
}

size_t BuffAbstract::calculate_mem()
{
    size_t block_size {4096};
    size_t reserve_bl_count {1};
    // Base mem reserv calculate. 1 block for data needeng + 1 free block.
    return ((_top_offset + size_filled()) / block_size + reserve_bl_count) * block_size;
}

void BuffAbstract::when_reseted()
{

}

size_t BuffAbstract::top_offset() const
{
    return _top_offset;
}
