#include "AEvRedisMod.hpp"
#include "../../Config/GlobalConf.hpp"
#include "../../Logger/Logger.hpp"

namespace aev {

AEvRedisMod::AEvRedisMod(AEvStrandPtr main_loop_)
    :_ev_loop(std::move(main_loop_)),
     _socket(_ev_loop->get_io_service())
{

}

void AEvRedisMod::async_connect(const std::string &ip_, const unsigned port_, ConnCallback cb_)
{
    if (_connected)
        throw std::logic_error("Connected to Redis db. Can't do async_connect.");

    asio::error_code ec;
    asio::ip::address ip(asio::ip::address::from_string(ip_, ec));
    if (ec) {
        cb_(ec);
        return;
    }

    auto connection_handler = [this, cb_](asio::error_code ec)
    {
        cb_(ec);

        if (ec)
            return;

        cb_(ec);
        _connected = true;
        _resp_proc_running = true;
        __resp_proc();

    };
    _endpoint = asio::ip::tcp::endpoint(ip, port_);
    _socket.async_connect(_endpoint, _ev_loop->wrap(connection_handler));

}

void AEvRedisMod::async_disconnect(AEvRedisMod::ConfirmCallback cb_)
{
    if (!_connected) {
        cb_(true);
        return;
    }
    async_stop([this, cb_](bool confirm)
    {
        if (!confirm) {
            cb_(false);
            return;
        }
        _socket.close();
        _connected = false;

        _cb_queue = redis::RedisCallbackQueue();
#ifdef AEV_REDIS_IN_MULTITHREADING
        // In thread working only io_service::run() with _socket async IO.
        // After _socket.close() - io_service::run() must return immediately.
        // But to follow the principle of of asynchrony - call detach().
        if (procs_tread.joinable())
            procs_tread.detach();
#endif
        cb_(true);
    }
                );
}

void AEvRedisMod::async_stop(AEvRedisMod::ConfirmCallback cb_)
{
    if (!_connected) {
        cb_(true);
        return;
    }

     _wait_and_stop = true;
     _cb_stop = cb_;
}

void AEvRedisMod::async_query(const std::string &query_, redis::RedisCallback cb_)
{

    if (!_connected)
        throw std::logic_error("Not connected to Redis db. Can't do async_query");

    if (_wait_and_stop)
        throw std::logic_error("Stop process inited. Query ignored.");

    {
#ifdef AEV_REDIS_IN_MULTITHREADING
        std::lock_guard<std::mutex> lock(sbuff_locker);
#endif
        _sending_buff << query_;
        _sending_buff << "\r\n";
        {
#ifdef AEV_REDIS_IN_MULTITHREADING
            std::lock_guard<std::mutex> lock(queue_locker);
#endif
            _cb_queue.push(cb_);
        }
    }
    if (_req_proc_running)
        return;

    _req_proc_running = true;
    __req_poc();
}

void AEvRedisMod::__req_poc()
{
#ifdef AEV_REDIS_IN_MULTITHREADING
    std::lock_guard<std::mutex> lock(sbuff_locker);
#endif // AEV_REDIS_IN_MULTITHREADING
    if (_sending_buff.nothing_to_send()) {
        _req_proc_running = false;
        return;
    }
    auto req_handler = [this](std::error_code ec, std::size_t bytes_sent)
    {
        if (ec) {
            _req_proc_running = false;
            return;
        }
        {
#ifdef AEV_REDIS_IN_MULTITHREADING
            std::lock_guard<std::mutex> lock(sbuff_locker);
#endif // AEV_REDIS_IN_MULTITHREADING
            _sending_buff.sending_report(bytes_sent);
        }
        if (!_sending_buff.nothing_to_send()) {
            __req_poc();
            if (!_resp_proc_running)
                __resp_proc();
        }
        else
            _req_proc_running = false;
    };

    _socket.async_send(asio::buffer(_sending_buff.new_data(), _sending_buff.new_data_size()), _ev_loop->wrap(req_handler));

}

void AEvRedisMod::__resp_proc()
{
    _reading_buff.release(256);
    auto resp_handler = [this](std::error_code ec, std::size_t bytes_sent)
    {
          if (ec)
              throw std::logic_error(Log::glob().log_format_to_str("Resp processor error. Err num: %, message: %", ec.value(), ec.message()));

          _reading_buff.accept(bytes_sent);
          {
              while (_reading_buff.parse_one(_respond))
              {
                  // Critical error. Answer resived, but no one callback in queue.
                  if (!_cb_queue.size())
                      throw std::logic_error("No one callbacks(11). Query/resp processors sync error.");

                  // Call client function.
                  _cb_queue.front()(0, _respond);

                  // Lock queue for pop first.
                  {
#ifdef AEV_REDIS_IN_MULTITHREADING
                      std::lock_guard<std::mutex> lock(queue_locker);
#endif
                      _cb_queue.pop();
                  }
                  // All queryes are complated, query processor not runing and client initiated waiting
                  // for complation all queryes.
                  if (!_cb_queue.size() && _wait_and_stop && !_req_proc_running) {
                      _resp_proc_running = false;
                      if (_cb_stop) {
                          _cb_stop(true);
                          _cb_stop = nullptr;
                      }
                      return;
                  }

              }
          }
          __resp_proc();
      };
    _socket.async_read_some(asio::buffer(_reading_buff.data_top(), _reading_buff.size_avail()), _ev_loop->wrap(resp_handler));
}

void AEvRedisMod::start()
{
    if (!_connected)
        throw std::logic_error("Not connected to Redis db. Can't start.");

    _wait_and_stop = false;

    if (_cb_stop) {
        _cb_stop(false);
        _cb_stop = nullptr;
    }

    if (!_resp_proc_running)
        __resp_proc();
}

/// Blocking operations. Only for "procs in separate thread". (AEV_REDIS_IN_MULTITHREADING def)
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
    _resp_proc_running = true;
    __resp_proc();

    procs_tread = std::thread([this](){_ev_loop->get_io_service().run();});
    return true;
}

void AEvRedisMod::stop()
{
    if (!_connected)
        throw std::logic_error("Not connected to Redis db. Can't do stop.");

    _wait_and_stop = true;
    while (true)
    {
        if (!_resp_proc_running)
            break;
    }
    if (_cb_stop) {
        _cb_stop(true);
        _cb_stop = nullptr;
    }
}

void AEvRedisMod::disconnect()
{
    if (!_connected)
        return;

    stop();
    _socket.close();
    _connected = false;

    _cb_queue = redis::RedisCallbackQueue();
    if (procs_tread.joinable())
        procs_tread.join();
}

#endif
} // namespace
