#ifndef AEVREDISMOD_HPP
#define AEVREDISMOD_HPP

#include "../../Atomic/AtomicTypes.hpp"
#include "../AEvBase/AEventTypes.hpp"
#include "RESPBuffer.hpp"
#include "RESProto.hpp"
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

    bool connect(const std::string & ip_, const unsigned port_);
    bool connect(const std::string & ip_, const unsigned port_, asio::error_code & ec_);
    void disconnect();


private:
    AEvStrandPtr _ev_loop;
    asio::ip::tcp::socket _socket;
    asio::ip::tcp::endpoint _endpoint;
    RESProto _resp_parser;
    RESPBuffer & _reading_buff;
    RedisQueryBuff _sending_buff;
    redis::RespData _respond;

    redis::FnHandlerQueue<void (int, const redis::RespData &)> _cb_queue;
    redis::FnHandlerQueue<void ()> _disconnection_waiters;

    std::mutex _send_buff_mux;
    std::atomic<bool> _req_proc_running {false};
    std::atomic<bool> _stop_in_progress {true};

    std::promise<void> _work_done_waiter;
    std::thread _procs_tread;

    bool _connected {false};

    void stop();
    void run_procs_thread();
    void work_done_report();

    void __req_poc();
    void __req_proc_manager();
    void __resp_proc();
};

} // namespace aev

#endif // AEVREDISMOD_HPP
