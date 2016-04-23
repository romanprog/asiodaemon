#ifndef AEvRedisCli_HPP
#define AEvRedisCli_HPP

#include "../AEvBase/AEventAbstract.hpp"
#include "RESProto.hpp"
#include "RedisBuffer.hpp"

namespace aev {

using RedisRetFunc = std::function<void (int err, redis::RespDataPtr&& result)>;

class AEvRedisCli : public AEventAbstract
{
public:
    explicit AEvRedisCli(AEvChildConf &&config, std::string query, RedisRetFunc ret_func);
    ~AEvRedisCli();
private:

    asio::ip::tcp::socket _socket;
    std::string _query;
    asio::ip::tcp::endpoint endpoint;
    RESProto _proto_parser;
    RedisRetFunc ret_function_cb;
    int err {0};

    virtual void _ev_begin() override;
    virtual void _ev_finish() override;
    virtual void _ev_stop() override;
    virtual void _ev_timeout() override;
    virtual void _ev_child_callback(AEvPtrBase child_ptr, AEvExitSignal & _ret) override;
    void _connect();
    void _send_request();
    void _get_respond();

};

} //namespace

#endif // AEvRedisCli_HPP
