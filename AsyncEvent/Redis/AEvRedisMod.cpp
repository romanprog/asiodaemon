#include "AEvRedisMod.hpp"
#include "../../Config/GlobalConf.hpp"
#include "../../Logger/Logger.hpp"

namespace aev {

AEvRedisMod::AEvRedisMod(AEvStrandPtr main_loop_)
    : _ev_loop(std::move(main_loop_)),
      _socket(_ev_loop->get_io_service())
{

}

AEvRedisMod::~AEvRedisMod()
{
    disconnect();
    if (procs_tread.joinable())
        procs_tread.join();
}

void AEvRedisMod::async_connect(const std::string &ip_, const unsigned port_, ConnCallback cb_)
{
    if (_connected)
        throw std::logic_error("Already connected to Redis db. Can't do async_connect.");

    asio::error_code ec;
    asio::ip::address ip(asio::ip::address::from_string(ip_, ec));
    if (ec) {
        cb_(ec);
        return;
    }

    auto connection_handler = [this, cb_](asio::error_code ec)
    {
        if (ec) {
            cb_(ec);
            return;
        }

        _connected = true;
        _stop_in_progress = false;

        __resp_proc();
        cb_(ec);
    };

    _endpoint = asio::ip::tcp::endpoint(ip, port_);
    _socket.async_connect(_endpoint, _ev_loop->wrap(connection_handler));
}

void AEvRedisMod::async_disconnect(AEvRedisMod::ConfirmCallback cb_)
{
    if (!_connected) {
        cb_();
        return;
    }
    _stop_in_progress = true;

    _disconnect_waiters.push([this, cb_]()
    {
        if (_connected) {
            _connected = false;
            _socket.close();
        }

        _cb_queue.clear();
        cb_();
    });
}

void AEvRedisMod::async_query(const std::string &query_, redis::RedisCallback cb_)
{

    if (!_connected) {
        throw std::logic_error("Not connected to Redis db. Can't do async_query");
    }
    if (_stop_in_progress) {
        redis::RespData resp;
        resp.sres = "Async processors stoped. Query ignored.";
        cb_(100, resp);
    }

    {
#ifdef AEV_REDIS_IN_MULTITHREADING
        std::lock_guard<std::mutex> lock(_send_buff_mux);
#endif // AEV_REDIS_IN_MULTITHREADING
        _cb_queue.push(cb_);
        _sending_buff << query_;
        _sending_buff << "\r\n";

    }
    _req_proc_manager();
}

void AEvRedisMod::__req_poc()
{

    auto req_handler = [this](std::error_code ec, std::size_t bytes_sent)
    {
        if (!ec) {
            std::lock_guard<std::mutex> lock(_send_buff_mux);
            _sending_buff.sending_report(bytes_sent);
        }

        _req_proc_running = false;
        _req_proc_manager();

    };
    _socket.async_write_some(asio::buffer(_sending_buff.new_data(), _sending_buff.new_data_size()), _ev_loop->wrap(req_handler));
}

void AEvRedisMod::_req_proc_manager()
{
    std::lock_guard<std::mutex> lock_req_check (_req_proc_mux);

    if (_req_proc_running)
        return;

    if (_sending_buff.nothing_to_send()) {
        return;
    }

    _req_proc_running = true;
    __req_poc();

}

void AEvRedisMod::__resp_proc()
{
    _reading_buff.release(2048);
    auto resp_handler = [this](std::error_code ec, std::size_t bytes_sent)
    {
          if (ec) {
              // log_debug("Resp proc error. May be stoped. % %", ec.value(), ec.message());
              return;
          }
          _reading_buff.accept(bytes_sent);

          while (_reading_buff.parse_one(_respond))
          {
              // Critical error. Answer resived, but no one callback in queue.
              if (_cb_queue.empty())
                  throw std::logic_error("No one callbacks(11). Query/resp processors sync error.");

              // Call client function.
              _cb_queue.call_and_pop(0, _respond);

              if (_stop_in_progress && _cb_queue.empty())
                  __work_done();

          }
          __resp_proc();
      };

    _socket.async_read_some(asio::buffer(_reading_buff.data_top(), _reading_buff.size_avail()), _ev_loop->wrap(resp_handler));
}

void AEvRedisMod::__run_procs_thread()
{
    procs_tread = std::thread(
                [this]()
    {
        _ev_loop->get_io_service().run();
    });
}

void AEvRedisMod::__work_done()
{
    _work_done_waiter.set_value();
    _disconnect_waiters.call_and_pop_all();
}


/// Blocking operations. Only for "procs in separate thread". (AEV_REDIS_IN_MULTITHREADING define)
#ifdef AEV_REDIS_IN_MULTITHREADING

bool AEvRedisMod::connect(const std::string &ip_, const unsigned port_)
{
    asio::error_code ec;
    return connect(ip_, port_, ec);
}

bool AEvRedisMod::connect(const std::string &ip_, const unsigned port_, asio::error_code &ec_)
{
    if (_connected)
        throw std::logic_error("Connected to Redis db. Can't do connect.");

    asio::ip::address ip(asio::ip::address::from_string(ip_, ec_));
    if (ec_)
        return false;

    _endpoint = asio::ip::tcp::endpoint(ip, port_);
    _socket.connect(_endpoint, ec_);

    if (ec_)
        return false;

    _connected = true;
    _stop_in_progress = false;

    __resp_proc();
    __run_procs_thread();
    return true;
}

void AEvRedisMod::stop()
{
    _stop_in_progress = true;
    if (!_cb_queue.empty()) {
        auto _local_waiter = _work_done_waiter.get_future().share();
        _local_waiter.wait();
    }
}

void AEvRedisMod::disconnect()
{
    if (!_connected)
        return;
    _connected = false;

    stop();
    _socket.close();
    log_main("Connection closed.");


    _cb_queue.clear();
    if (procs_tread.joinable())
        procs_tread.join();
}
#endif // AEV_REDIS_IN_MULTITHREADING
} // namespace
