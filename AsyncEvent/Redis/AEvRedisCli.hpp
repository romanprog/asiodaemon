#ifndef AEvRedisCli_HPP
#define AEvRedisCli_HPP

#include "../AEvBase/AEventAbstract.hpp"
#include "RedisBuffer.hpp"

namespace aev {

using RetFunc = std::function<void (int err, dns::DnsRespondPtr && result)>;

class AEvRedisCli : public AEventAbstract
{
public:
    explicit AEvRedisCli(AEvChildConf &&config, std::string query, RetFunc ret_func);
    ~AEvRedisCli();
private:

    asio::ip::udp::socket _socket;
    std::string _query;
    asio::ip::tcp::endpoint endpoint;
    DnsBuffer buff;
    RetFunc ret_function_cb;
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

#endif // AEvRedisCli_HPP
