#ifndef AEVREDISMOD_HPP
#define AEVREDISMOD_HPP

#define AEV_REDIS_IN_MULTITHREADING

#include "../AEvBase/AEventTypes.hpp"
#include "RedisBuffer.hpp"
#include "mutex"
#include "thread"

namespace aev {


class AEvRedisMod
{
public:

    AEvRedisMod(AEvStrandPtr main_loop_);

    using ConnCallback = std::function<void (asio::error_code&)>;
    using ConfirmCallback = std::function<void (bool succes)>;
    void async_connect(const std::string & ip_, const unsigned port_, ConnCallback cb_);
    void async_disconnect(ConfirmCallback cb_);
    void async_stop(ConfirmCallback cb_);
    void async_query(const std::string & query_,  redis::RedisCallback cb_);
    // start() is non blocking method. Acceptable for single-thread and multi-thread model.
    void start();

#ifdef AEV_REDIS_IN_MULTITHREADING
    bool connect(const std::string & ip_, const unsigned port_);
    bool connect(const std::string & ip_, const unsigned port_, asio::error_code & ec_);
    void stop();

    void disconnect();
#endif

private:
    AEvStrandPtr _ev_loop;
    asio::ip::tcp::socket _socket;
    asio::ip::tcp::endpoint _endpoint;
    RedisRBuffer _reading_buff;
    RedisWBuffer _sending_buff;
    redis::RedisCallbackQueue _cb_queue;
    redis::RespData _respond;
    ConfirmCallback _cb_stop {nullptr};

#ifdef AEV_REDIS_IN_MULTITHREADING
    std::mutex queue_locker, sbuff_locker;
    std::atomic<bool> _req_proc_running {false};
    std::atomic<bool> _wait_and_stop {false};
    std::thread procs_tread;
# else
    bool _req_proc_running {false};
    bool _wait_and_stop {false};
#endif
    bool _connected {false};

    bool _resp_proc_running {false};

    void __req_poc();
    void __resp_proc();

};

} // namespace aev

#endif // AEVREDISMOD_HPP
