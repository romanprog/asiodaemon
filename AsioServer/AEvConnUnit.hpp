#ifndef AEVCONNUNIT_HPP
#define AEVCONNUNIT_HPP

#include "AEventsAbstract.hpp"
#include "AEvBuffer.hpp"

namespace aev {

class AEvConnUnit : public AEventsAbstract
{
public:

    explicit AEvConnUnit(const AEvChildConf config, asio::ip::tcp::socket _soc);
    virtual ~AEvConnUnit() override
    {
        // std::cout << "AEvConnUnit DESTRUCTOR! " << std::endl;
    }

private:

    asio::ip::tcp::socket _socket;
    std::array<char, 8192> _buffer;
    AEvBuffer _read_buf;

protected:
    virtual void _ev_begin() override;
    virtual void _ev_finish() override;
    virtual void _ev_stop() override;
    virtual void _ev_timeout() override;
    virtual void _ev_child_callback(AEvExitSignal _ret) override;

    void _start_read();

};

} //namespace

#endif // AEVCONNUNIT_HPP
