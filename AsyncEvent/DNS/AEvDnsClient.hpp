#ifndef AEVDNSCLIENT_HPP
#define AEVDNSCLIENT_HPP

#include "../AEvBase/AEventAbstract.hpp"
#include "DnsBuffer.hpp"

namespace aev {



using RetFunc = std::function<void (int err, dns::DnsRespondPtr && result)>;

class AEvDnsClient : public AEventAbstract
{
public:
    explicit AEvDnsClient(AEvChildConf &&config, std::string name, dns::DnsQType qt, RetFunc ret_func);
    ~AEvDnsClient();
private:

    asio::ip::udp::socket _socket;
    std::string _domain;
    asio::ip::udp::endpoint endpoint;
    DnsBuffer buff;
    RetFunc ret_function_cb;
    dns::DnsQType query_type;
    dns::DnsError err {dns::DnsError::noerror};

    virtual void _ev_begin() override;
    virtual void _ev_finish() override;
    virtual void _ev_stop() override;
    virtual void _ev_timeout() override;
    virtual void _ev_child_callback(AEvPtrBase child_ptr, AEvExitSignal & _ret) override;

    void _send_request();
    void _get_respond();

};

} //namespace

#endif // AEVDNSCLIENT_HPP
