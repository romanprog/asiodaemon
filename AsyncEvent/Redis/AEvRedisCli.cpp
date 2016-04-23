#include "AEvRedisCli.hpp"
#include "RedisBuffer.hpp"
#include "../../Config/GlobalConf.hpp"

#include "iostream"

namespace aev {

AEvRedisCli::AEvRedisCli(AEvChildConf && config, std::string query, RedisRetFunc ret_func)
    :AEventAbstract::AEventAbstract(std::move(config)),
     _socket(_ev_loop->get_io_service()),
     _query(std::move(query)),
     endpoint(asio::ip::address::from_string(Config::glob().get_conf().redis_host), Config::glob().get_conf().redis_port),
     ret_function_cb(ret_func)

{
    _query += "\r\n";
    log_debug("AEvRedisCli CONSTRUCTOR! ");
}

AEvRedisCli::~AEvRedisCli()
{
    log_debug("~AEvRedisCli DESTRUCTOR");
}

void AEvRedisCli::_ev_begin()
{
    log_debug("AEvRedisCli START");
    _connect();
}

void AEvRedisCli::_ev_finish()
{
    log_debug("AEvRedisCli FINISH" );
    _socket.close();
//    ret_function_cb(err, buff.);
}

void AEvRedisCli::_ev_stop()
{
    // close() will cancelled immediately all scync operations.
    log_debug("AEvRedisCli STOP");
}

void AEvRedisCli::_ev_timeout()
{
    log_debug("AEvRedisCli TIMEOUT");
    err = 1;
}



void AEvRedisCli::_send_request()
{

    _socket.async_send(asio::buffer(_query),
                          _ev_loop->wrap([this](std::error_code ec, std::size_t bytes_sent)
    {
                              if (ec) {
                                  stop();
                                  return;
                              }
                              log_debug("sended Redis Request");
                              _get_respond();
                          })
            );
}

void AEvRedisCli::_get_respond()
{
    _proto_parser.buff().release(2048);
    _socket.async_read_some(asio::buffer(_proto_parser.buff().data_top(), _proto_parser.buff().size_avail()),
                                   _ev_loop->wrap([this](std::error_code ec, std::size_t bytes_sent)
        {
                                       if (ec) {
                                           log_debug("Receive error %", ec.message());
                                           stop();
                                           return;
                                       }
                                       log_debug("received Redis Respond");
                                       _proto_parser.buff().accept(bytes_sent);

//                                       if (!buff.is_complate())
//                                            _get_respond();

                                       stop();
                                   })
                );

}

void aev::AEvRedisCli::_ev_child_callback(AEvPtrBase child_ptr, AEvExitSignal &_ret)
{

}

void AEvRedisCli::_connect()
{
    log_debug("AEvRedisCli Try to connect");
    _socket.async_connect(endpoint,
                          _ev_loop->wrap(
                        [this](std::error_code ec)
                        {
                              if (ec) {
                                  log_debug("Connection error! % ", ec.message());
                                  stop();
                                  return;
                              }
                              _send_request();

                          }
                              ));
}


} //namespace
