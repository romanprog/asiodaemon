#ifndef AEVDNSCLIENT_HPP
#define AEVDNSCLIENT_HPP

#include "../Abstract/AEventAbstract.hpp"
#include "../../Buffers/DnsBuffer.hpp"

namespace aev {

class AEvDnsClient : public AEventAbstract
{
public:
    explicit AEvDnsClient(const AEvChildConf config, std::string dom);

private:

    asio::ip::udp::socket _socket;
    std::string _domain;

protected:
    virtual void _ev_begin() override;
    virtual void _ev_finish() override;
    virtual void _ev_stop() override;
    virtual void _ev_timeout() override;
    virtual void _ev_child_callback(AEvPtrBase child_ptr, AEvExitSignal & _ret) override;

    void _resolve();

};

} //namespace

#endif // AEVDNSCLIENT_HPP
