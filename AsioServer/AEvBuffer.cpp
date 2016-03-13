#include "AEvBuffer.hpp"

AEvBuffer::AEvBuffer()
    :_size(_blocks_reserved * _block_size)
{
    _data = malloc(_size);
    _reserved = _size;
}

AEvBuffer::AEvBuffer(size_t sz)
    :_size(sz)
{
    _reserved = (_size/_block_size+_blocks_reserved)*_block_size;
    _data = malloc(_reserved);
}

AEvBuffer::~AEvBuffer()
{
    free(_data);
}

void AEvBuffer::expand(size_t size)
{
    if ((size+_size)>=_reserved) {
        _reserved = ((_size+size)/_block_size+_blocks_reserved)*_block_size;
        _data = realloc(_data, _reserved);
    }

    _size=_size+size;
}
