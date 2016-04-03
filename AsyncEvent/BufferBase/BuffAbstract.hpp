#ifndef BUFFABSTRACT_HPP
#define BUFFABSTRACT_HPP

#include <cstring>
#include <string>
#include <vector>

class BuffAbstract
{
public:
    BuffAbstract();
    virtual ~BuffAbstract();

    // Return pointer to first byte of read-only buffer data.
    const char * data() const;

    // Return pointer to first byte of free space.
    void * data_top();

    // Return pointer to first byte of read-write buffer data.
    void * vdata();

    // Public method to call after any data writed in buffer.
    // Call virtual @when_new_data_acc.
    bool accept(size_t bytes_readed);

    // Allocates a specified number of bytes of free space.
    // Call virtual method @calculate_mem to reserve additional memory for next @release call.
    // If reserved memory enough - new memory is not allocated.
    void release(size_t size__);

    // Reset buffer, clear data, and ser all counters to default.
    // If needed override this function - make sure to call @BuffAbstract::reset() in derived class!
    virtual void reset();

    // In base variant - return full size of buffer @_size (size_filled() + size_avail()). Can be overridden in derived classes.
    virtual size_t size() const;

    // Return actual data size, stored in buffer.
    size_t size_filled() const;

    // Return size of free buffer space, allowing to store new data.
    size_t size_avail() const;

    template <typename T>
    void operator << (const T & obj)
    {
        release(sizeof(T));
        memcpy(data_top(), &obj, sizeof(T));
        accept(sizeof(T));
    }

    void operator << (const std::string & str);

protected:
    virtual size_t calculate_mem();
    virtual void when_new_data_acc(size_t bytes_readed) = 0;
    virtual void when_reseted();
    size_t top_offset() const;

private:
    size_t _top_offset {0};
    size_t _reserved {0};
    size_t _size {0};
    size_t _data_cursor{0};
    char * _cdata;

};

#endif // BUFFABSTRACT_HPP
