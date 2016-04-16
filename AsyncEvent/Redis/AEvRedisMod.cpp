#include "AEvRedisMod.hpp"
#include "../../Config/GlobalConf.hpp"
#include "../../Logger/Logger.hpp"

namespace aev {

AEvRedisMod::AEvRedisMod(AEvStrandPtr _main_loop)
    :_ev_loop(std::move(_main_loop)),
     _socket(_ev_loop->get_io_service()),
     redis_ip(Config::glob().get_conf().redis_host),
     redis_port(Config::glob().get_conf().redis_port)
{

}

AEvRedisMod::AEvRedisMod(AEvStrandPtr _main_loop, const std::string &ip, int port)
    :_ev_loop(std::move(_main_loop)),
     _socket(_ev_loop->get_io_service()),
     redis_ip(ip),
     redis_port(port)
{

}

void AEvRedisMod::query_processor()
{

    auto sender_handler = [this](std::error_code ec, std::size_t bytes_sent)
    {
          if (ec) {
              log_debug("Send error %", ec.message());
              {
                  std::lock_guard<std::mutex> lock(queue_locker);
                  _cb_queue.front()(1000, _respond);
                  _cb_queue.pop();
              }
          }

          if (!_query_queue.empty())
              query_processor();
          else
              is_busy = false;

    };

    _socket.async_send(asio::buffer(_query_queue.front()), _ev_loop->wrap(sender_handler));

    std::lock_guard<std::mutex> lock(queue_locker);
    _query_queue.pop();
}

void AEvRedisMod::async_query(std::string query, redis::RedisCallback cb)
{
    if (!connected) {
        cb(1, _respond);
        return;
    }

    query+="\r\n";
    {
        std::lock_guard<std::mutex> lock(queue_locker);
        _query_queue.push(std::move(query));
        _cb_queue.push(cb);
    }
    if (is_busy)
        return;

    is_busy = true;
    query_processor();
}

void AEvRedisMod::stop()
{
    _socket.close();
    while (!_cb_queue.empty())
    {
        _cb_queue.front()(100, _respond);
        _cb_queue.pop();
    }
    while (!_query_queue.empty())
        _query_queue.pop();
    _query_queue = std::queue<std::string>();
    _cb_queue = redis::RedisCallbackQueue();
    connected = false;
}

void AEvRedisMod::async_connect(std::function<void (int)> cb)
{
    asio::error_code ec;
    asio::ip::address ip(asio::ip::address::from_string(redis_ip, ec));
    if (ec) {
        error_handler(ec.message());
        cb(ec.value());
        return;
    }

    auto connection_handler = [this, cb](asio::error_code ec)
    {
        if (ec)
        {
            error_handler(ec.message());
            cb(ec.value());
            return;
        }
        connected = true;
        cb(0);
        resp_processor();

    };
    endpoint = asio::ip::tcp::endpoint(ip, redis_port);
    _socket.async_connect(endpoint, _ev_loop->wrap(connection_handler));

}

void AEvRedisMod::query_loop(std::string query, redis::RedisCallback cb, int count)
{
    query+="\r\n";
    loop_counter = count;
    loop_query = std::move(query);
    _loop_cb = cb;

    query_loop_procc();

}

void AEvRedisMod::query_loop_procc()
{
    auto sender_handler = [this](std::error_code ec, std::size_t bytes_sent)
    {
          if (ec)
                return;

          --loop_counter;
          if (!loop_counter) {
              exit(0);
              return;
          }

          query_loop_procc();
    };

    _socket.async_send(asio::buffer(loop_query.data(), loop_query.size()), sender_handler);
}

bool AEvRedisMod::connect()
{
    asio::error_code ec;
    asio::ip::address ip(asio::ip::address::from_string(redis_ip, ec));
    if (ec) {
        error_handler(ec.message());
        return false;
    }
    endpoint = asio::ip::tcp::endpoint(ip, redis_port);
    _socket.connect(endpoint, ec);
    if (ec) {
        error_handler(ec.message());
        return false;
    }
    connected = true;
    resp_processor();
    return true;
}

void AEvRedisMod::error_handler(const std::string msg)
{
    log_main("Error message: %", msg);
}

void AEvRedisMod::resp_processor()
{
    buff.release(256);
    auto resp_handler = [this](std::error_code ec, std::size_t bytes_sent)
    {
          if (ec) {
              log_debug("Receive error %", ec.message());

              return;
          }

          buff.accept(bytes_sent);

          while (buff.parse_one(_respond))
          {
//              {
//                  std::lock_guard<std::mutex> lock(queue_locker);
//                  _cb_queue.front()(0, _respond);
//                  _cb_queue.pop();
//              }
              _loop_cb(0, _respond);

          }

          resp_processor();

      };
    _socket.async_read_some(asio::buffer(buff.data_top(), buff.size_avail()), _ev_loop->wrap(resp_handler));
}

} // namespace
