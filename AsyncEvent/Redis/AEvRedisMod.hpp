#ifndef AEVREDISMOD_HPP
#define AEVREDISMOD_HPP

#define AEV_REDIS_IN_MULTITHREADING

#include "../../Atomic/AtomicTypes.hpp"
#include "../AEvBase/AEventTypes.hpp"
#include "RedisBuffer.hpp"
#include <mutex>
#include <thread>
#include <future>

namespace aev {


class AEvRedisMod
{
public:

    AEvRedisMod(AEvStrandPtr main_loop_);
    ~AEvRedisMod();

    using ConnCallback = std::function<void (asio::error_code&)>;
    using ConfirmCallback = std::function<void ()>;

    void async_connect(const std::string & ip_, const unsigned port_, ConnCallback cb_);
    void async_disconnect(ConfirmCallback cb_);

    void async_query(const std::string &query_,  redis::RedisCallback cb_);

#ifdef AEV_REDIS_IN_MULTITHREADING
    bool connect(const std::string & ip_, const unsigned port_);
    bool connect(const std::string & ip_, const unsigned port_, asio::error_code & ec_);
    void disconnect();
#endif

private:
    AEvStrandPtr _ev_loop;
    asio::ip::tcp::socket _socket;
    asio::ip::tcp::endpoint _endpoint;
    RedisRBuffer _reading_buff;
    RedisWBuffer _sending_buff;
    redis::RespData _respond;

    redis::FnHandlerQueue<void (int, const redis::RespData &)> _cb_queue;
    redis::FnHandlerQueue<void ()> _disconnect_waiters;

#ifdef AEV_REDIS_IN_MULTITHREADING
    std::mutex _req_proc_mux, _send_buff_mux;
    std::atomic<bool> _req_proc_running {false};
    std::atomic<bool> _io_runing {false};
    std::atomic<bool> _stop_in_progress {true};
    std::promise<void> _work_done_waiter;
    std::thread procs_tread;
# else
    bool _req_proc_running {false};
    bool _resp_proc_running {false};
#endif

    bool _connected {false};
    void stop();

    void __req_poc();
    void _req_proc_manager();
    void __resp_proc();
    void __run_procs_thread();
    void __work_done();

};

} // namespace aev

#endif // AEVREDISMOD_HPP
