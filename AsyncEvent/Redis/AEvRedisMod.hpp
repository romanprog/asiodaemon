#ifndef AEVREDISMOD_HPP
#define AEVREDISMOD_HPP

#include "../AEvBase/AEventTypes.hpp"
#include "RedisBuffer.hpp"
#include "mutex"

namespace aev {


class AEvRedisMod
{
public:
    AEvRedisMod(AEvStrandPtr _main_loop);
    AEvRedisMod(AEvStrandPtr _main_loop, const std::string & ip, int port);

    void async_query(std::string query,  redis::RedisCallback cb);
    void stop();
    void async_connect(std::function<void(int)> cb);
    void query_loop(std::string query,  redis::RedisCallback cb, int count);
    void query_loop_procc();
    bool connect();

private:
    AEvStrandPtr _ev_loop;
    asio::ip::tcp::socket _socket;
    asio::ip::tcp::endpoint endpoint;
    RedisBuffer buff;
    redis::RedisCallbackQueue _cb_queue;
    std::queue<std::string> _query_queue;
    redis::RespData _respond;
    std::string redis_ip;
    int redis_port;

    redis::RedisCallback _loop_cb;
    std::string loop_query;
    std::atomic<int> loop_counter {0};

    std::mutex queue_locker;

    std::string error_message;
    int error_code;

    bool connected {false};
    bool is_busy {false};

    void error_handler(const std::string msg);
    void query_processor();
    void resp_processor();
    int err {0};

};

} // namespace aev

#endif // AEVREDISMOD_HPP
