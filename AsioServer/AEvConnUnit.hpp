#ifndef AEVCONNUNIT_HPP
#define AEVCONNUNIT_HPP

#include "AEventsAbstract.hpp"
#include "AEvBuffer.hpp"

namespace aev {

class AEvConnUnit : public AEventsAbstract
{
public:

    explicit AEvConnUnit(const AEvChildConf config, asio::ip::tcp::socket _soc);
    virtual ~AEvConnUnit() {
        // std::cout << "AEvConnUnit DESTRUCTOR! " << std::endl;
    }

private:

    asio::ip::tcp::socket _socket;
    std::array<char, 8192> _buffer;
    AEvBuffer _read_buf;

protected:
    virtual void _ev_begin();
    virtual void _ev_finish();
    virtual void _ev_stop();
    virtual void _ev_timeout();
    virtual void _ev_child_callback(AEvExitSignal _ret);

    void _start_read();

};

} //namespace

#endif // AEVCONNUNIT_HPP
