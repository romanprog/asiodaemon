#ifndef AEVREDISMOD_HPP
#define AEVREDISMOD_HPP

#include "../AEvBase/AEventTypes.hpp"
#include "RedisBuffer.hpp"
#include "mutex"

namespace aev {


class AEvRedisMod
{
public:

    AEvRedisMod(AEvStrandPtr main_loop_);

    using ConnCallback = std::function<void (asio::error_code&)>;
    bool connect(const std::string & ip_, const unsigned port_);
    bool connect(const std::string & ip_, const unsigned port_, asio::error_code & ec_);
    void async_connect(const std::string & ip_, const unsigned port_, ConnCallback cb_);
    void async_query(const std::string & query_,  redis::RedisCallback cb_);
    void wait();
    void disconnect();

private:
    AEvStrandPtr _ev_loop;
    asio::ip::tcp::socket _socket;
    asio::ip::tcp::endpoint _endpoint;
    RedisRBuffer _reading_buff;
    RedisWBuffer _sending_buff;
    redis::RedisCallbackQueue _cb_queue;
    redis::RespData _respond;

    std::mutex queue_locker, sbuff_locker;

    bool _connected {false};
    std::atomic<bool> _req_proc_running {false};
    bool _resp_proc_running {false};
    std::atomic<bool> _waiting_for_complation {false};

    void __req_poc();
    void __resp_proc();

};

} // namespace aev

#endif // AEVREDISMOD_HPP
