#ifndef BUFFABSTRACT_HPP
#define BUFFABSTRACT_HPP

#include <string>
#include <vector>

namespace aev {

class BuffAbstract
{
public:
    BuffAbstract();
    virtual ~BuffAbstract();

    // Return pointer to first byte of read-only buffer data.
    const char * data() const;

    // Return pointer to first byte of free space.
    void * data_top();
    bool accept(size_t bytes_readed);
    void release(size_t size__);

    // Reset buffer, clear data, and ser all counters to default.
    void reset();

    // In base variant - return size_avail(). Can be overridden in derived classes.
    virtual size_t size() const;

    // Return actual data size, stored in buffer.
    size_t size_filled() const;

    // Return size of free buffer space, allowing to store new data.
    size_t size_avail() const;

protected:
    virtual size_t calculate_mem();
    virtual void when_new_data_acc() = 0;
    virtual void when_reseted() = 0;

    size_t _reserved;
    size_t _size {0};
    size_t _top_offset {0};
    size_t _data_cursor{0};
    char * _cdata;

};

}
#endif // BUFFABSTRACT_HPP
