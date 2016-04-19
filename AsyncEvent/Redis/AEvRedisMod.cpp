#include "AEvRedisMod.hpp"
#include "../../Config/GlobalConf.hpp"
#include "../../Logger/Logger.hpp"

namespace aev {

AEvRedisMod::AEvRedisMod(AEvStrandPtr main_loop_)
    :_ev_loop(std::move(main_loop_)),
     _socket(_ev_loop->get_io_service())
{

}

void AEvRedisMod::__req_poc()
{

    auto sender_handler = [this](std::error_code ec, std::size_t bytes_sent)
    {
          if (ec) {
              redis::RespData resp;
              resp.sres = ec.message();

              if (!_cb_queue.size())
                  throw std::logic_error("No one callbacks(12). Query/resp processors sync error.");

              _cb_queue.front()(ec.value(), resp);

              {
                  std::lock_guard<std::mutex> lock(queue_locker);
                  _cb_queue.pop();
              }

              if (!_cb_queue.size()) {
                  _req_proc_running = false;
                  return;
              }
          }
          {
              std::lock_guard<std::mutex> lock(sbuff_locker);
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
    std::lock_guard<std::mutex> lock(sbuff_locker);
    _socket.async_send(asio::buffer(_sending_buff.new_data(), _sending_buff.new_data_size()), _ev_loop->wrap(sender_handler));

}

void AEvRedisMod::async_query(const std::string &query_, redis::RedisCallback cb_)
{

    if (!_connected)
        throw std::logic_error("Not connected to Redis db.");

    if (_waiting_for_complation)
        throw std::logic_error("Stop process inited. Query ignored.");

    {
        std::lock_guard<std::mutex> lock(sbuff_locker);
        _sending_buff << query_;
        _sending_buff << "\r\n";
    }

    {
        std::lock_guard<std::mutex> lock(queue_locker);
        _cb_queue.push(cb_);
    }

    if (_req_proc_running.load())
        return;

    _req_proc_running.store(true);
    __req_poc();
}

void AEvRedisMod::disconnect()
{
    if (!_connected)
        return;

    wait();
    _socket.close();
    _cb_queue = redis::RedisCallbackQueue();

}

void AEvRedisMod::async_connect(const std::string &ip_, const unsigned port_, ConnCallback cb_)
{

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

void AEvRedisMod::wait()
{
    _waiting_for_complation = true;
    while (true)
    {
        if (!_resp_proc_running)
            break;
    }
}


bool AEvRedisMod::connect(const std::string &ip_, const unsigned port_)
{
    asio::error_code ec;
    return connect(ip_, port_, ec);
}

bool AEvRedisMod::connect(const std::string &ip_, const unsigned port_, asio::error_code &ec_)
{
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
    return true;
}

void AEvRedisMod::__resp_proc()
{
    _reading_buff.release(256);
    auto resp_handler = [this](std::error_code ec, std::size_t bytes_sent)
    {
          if (ec) {
              {
                  redis::RespData resp;
                  resp.sres = ec.message();
                  if (!_cb_queue.size())
                      throw std::logic_error("No one callbacks(10). Query/resp processors sync error.");

                  _cb_queue.front()(ec.value(), resp);
                  {
                      std::lock_guard<std::mutex> lock(queue_locker);
                      _cb_queue.pop();
                  }

                  if (!_cb_queue.size() && _waiting_for_complation && !_req_proc_running.load()) {
                      _resp_proc_running = false;
                      return;
                  };
               }
              __resp_proc();
              return;
          }

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
                      std::lock_guard<std::mutex> lock(queue_locker);
                      _cb_queue.pop();
                  }
                  // All queryes are complated, query processor not runing and client initiated waiting
                  // for complation all queryes.
                  if (!_cb_queue.size() && _waiting_for_complation && !_req_proc_running.load()) {
                      _resp_proc_running = false;
                      return;
                  }

              }
          }
          __resp_proc();
      };
    _socket.async_read_some(asio::buffer(_reading_buff.data_top(), _reading_buff.size_avail()), _ev_loop->wrap(resp_handler));
}

} // namespace
