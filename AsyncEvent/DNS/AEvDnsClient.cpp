#include "AEvDnsClient.hpp"
#include "DnsBuffer.hpp"

#include "iostream"

namespace aev {

AEvDnsClient::AEvDnsClient(AEvChildConf && config, std::string name, dns::DnsQType qt, DnsRetFunc ret_func)
    :AEventAbstract::AEventAbstract(std::move(config)),
     _socket(_ev_loop->get_io_service(), asio::ip::udp::endpoint(asio::ip::udp::v4(), 0)),
     _domain(name),
     ret_function_cb(ret_func),
     query_type(qt)
{
    log_debug("AEvDnsClient CONSTRUCTOR! ");
}

AEvDnsClient::~AEvDnsClient()
{
    log_debug("~AEvDnsClient DESTRUCTOR");
}

void AEvDnsClient::_ev_begin()
{
    log_debug("AEvDnsClient START");
    _send_request();
}

void AEvDnsClient::_ev_finish()
{
    log_debug("AEvDnsClient FINISH" );
    _socket.close();
    ret_function_cb(static_cast<int>(err), buff.withdraw_respond());
}

void AEvDnsClient::_ev_stop()
{
    // close() will cancelled immediately all scync operations.
    log_debug("AEvDnsClient STOP");
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
                          wrap_asio_cb([this](std::error_code ec, std::size_t bytes_sent)
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
                                   wrap_asio_cb([this](std::error_code ec, std::size_t bytes_sent)
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
