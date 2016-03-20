#ifndef AEVDNSCLIENT_HPP
#define AEVDNSCLIENT_HPP

#include "../Abstract/AEventAbstract.hpp"
#include "AEvBuffer.hpp"

namespace aev {

class AEvDnsClient : public AEventAbstract
{
public:
    explicit AEvDnsClient(const AEvChildConf config);

private:

    asio::ip::udp::socket _socket;
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

#endif // AEVDNSCLIENT_HPP
