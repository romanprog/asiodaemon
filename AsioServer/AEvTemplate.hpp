#ifndef AEVTEMPLATE_HPP
#define AEVTEMPLATE_HPP

#include "AEventsAbstract.hpp"
#include "AEvBuffer.hpp"

namespace aev {

class AEvTemplate : public AEventsAbstract
{
public:
    explicit AEvTemplate(const AEvChildConf config, asio::ip::tcp::socket _soc);

private:

    asio::ip::tcp::socket _socket;
    std::array<char, 8192> _buffer;
    AEvBuffer _read_buf;

protected:
    virtual void _ev_begin() override;
    virtual void _ev_finish() override;
    virtual void _ev_stop() override;
    virtual void _ev_timeout() override;
    virtual void _ev_child_callback(AEvPtrBase child_ptr, AEvExitSignal & _ret) override;

    void _start_read();

};

} //namespace
#endif // AEVTEMPLATE_HPP
