#ifndef AEVBUFFER_HPP
#define AEVBUFFER_HPP

#include <string>


class AEvBuffer
{
public:
    AEvBuffer();
    AEvBuffer(size_t sz);
    ~AEvBuffer();

    void * data()
    {
        return _data;
    }

    void expand(size_t size);

    size_t size()
    {
        return _size;
    }

    size_t reserved()
    {
        return _reserved;
    }

private:
    size_t _blocks_reserved {1};
    size_t _block_size {8192};
    size_t _cpos {0};

    std::string _separator {'\n'};
    size_t _size;
    size_t _reserved;

    void * _data;

};

#endif // AEVBUFFER_HPP
