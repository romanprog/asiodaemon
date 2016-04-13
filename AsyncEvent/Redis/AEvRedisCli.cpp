#include "AEvRedisCli.hpp"
#include "DnsBuffer.hpp"

#include "iostream"

namespace aev {

AEvRedisCli::AEvRedisCli(AEvChildConf && config, std::string query, RetFunc ret_func)
    :AEventAbstract::AEventAbstract(std::move(config)),
     _socket(_ev_loop->get_io_service(), asio::ip::tcp::endpoint(asio::ip::tcp::v4(), 0)),
     _query(std::move(query)),
     ret_function_cb(ret_func),

{
    log_debug("AEvRedisCli CONSTRUCTOR! ");
}

AEvRedisCli::~AEvRedisCli()
{
    log_debug("~AEvRedisCli DESTRUCTOR");
}

void AEvRedisCli::_ev_begin()
{
    log_debug("AEvRedisCli START");
    _send_request();
}

void AEvRedisCli::_ev_finish()
{
    log_debug("AEvRedisCli FINISH" );
    _socket.close();
    ret_function_cb(static_cast<int>(err), buff.withdraw_respond());
}

void AEvRedisCli::_ev_stop()
{
    // close() will cancelled immediately all scync operations.
    log_debug("AEvRedisCli STOP");
}

void AEvRedisCli::_ev_timeout()
{
    log_debug("AEvRedisCli TIMEOUT");
    err = dns::DnsError::timeout_err;
}


void AEvRedisCli::_send_request()
{

    asio::ip::udp::resolver resolver(_ev_loop->get_io_service());
    endpoint = *resolver.resolve({asio::ip::tcp::v4(), "127.0.1.1", "53"});

    if (!buff.prepare_for_request(_query, query_type))
    {
        stop();
        return;
    }
    _socket.async_send_to(asio::buffer(buff.data(), buff.size_filled()), endpoint,
                          _ev_loop->wrap([this](std::error_code ec, std::size_t bytes_sent)
    {
                              if (ec) {
                                  stop();
                                  return;
                              }
                              log_debug("sended DNS");
                              _get_respond();
                          })
            );
}

void AEvRedisCli::_get_respond()
{

        if (!buff.prepare_for_respond())
        {
            stop();
            return;
        }
        _socket.async_receive_from(asio::buffer(buff.data_top(), buff.size_avail()), endpoint,
                                   _ev_loop->wrap([this](std::error_code ec, std::size_t bytes_sent)
        {
                                       if (ec) {
                                           stop();
                                           return;
                                       }
                                       log_debug("receive DNS");
                                       buff.read_respond(bytes_sent);
                                       err = buff.get_error();
                                       stop();
                                   })
                );

}

void aev::AEvRedisCli::_ev_child_callback(AEvPtrBase child_ptr, AEvExitSignal &_ret)
{

}


} //namespace
