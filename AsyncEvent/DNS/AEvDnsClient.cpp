#include "AEvDnsClient.hpp"
#include "DnsBuffer.hpp"

#include "iostream"

namespace aev {

AEvDnsClient::AEvDnsClient(const AEvChildConf config, std::string name, dns::DnsQType qt, RetFunc ret_func)
    :AEventAbstract::AEventAbstract(config),
     _socket(config.evloop->get_io_service(), asio::ip::udp::endpoint(asio::ip::udp::v4(), 0)),
     _domain(name),
     ret_function_cb(ret_func),
     query_type(qt)
{
    log_debug("AEvConnection CONSTRUCTOR! ");
}

AEvDnsClient::~AEvDnsClient()
{
    log_debug("~AEvDnsClient()");
}

void AEvDnsClient::_ev_begin()
{
    log_debug("AEvDnsClient START");
    _send_request();
}

void AEvDnsClient::_ev_finish()
{
    log_debug("AEvDnsClient FINISH" );
}

void AEvDnsClient::_ev_stop()
{
    // close() will cancelled immediately all scync operations.
    log_debug("AEvDnsClient STOP");
    _socket.close();
    ret_function_cb(static_cast<int>(err), buff.get_respond());
}

void AEvDnsClient::_ev_timeout()
{
    log_debug("AEvDnsClient TIMEOUT");
    err = dns::DnsError::timeout_err;
}


void AEvDnsClient::_send_request()
{

    asio::ip::udp::resolver resolver(_ev_loop->get_io_service());
    endpoint = *resolver.resolve({asio::ip::udp::v4(), "127.0.1.1", "53"});

    if (!buff.prepare_for_request(_domain, query_type))
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

void AEvDnsClient::_get_respond()
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

void aev::AEvDnsClient::_ev_child_callback(AEvPtrBase child_ptr, AEvExitSignal &_ret)
{

}


} //namespace
