#include "AEvDnsClient.hpp"
#include "../../Buffers/DnsBuffer.hpp"

#include "iostream"

namespace aev {

AEvDnsClient::AEvDnsClient(const AEvChildConf config, std::string name, dns::DnsQType tp, RetFunc ret_func)
    :AEventAbstract::AEventAbstract(config),
     _socket(config.evloop->get_io_service(), asio::ip::udp::endpoint(asio::ip::udp::v4(), 0)),
     _domain(name),
     ret_function_cb(ret_func),
     query_type(tp)
{
    // std::cout << "AEvConnection CONSTRUCTOR! " << std::endl;
}

AEvDnsClient::~AEvDnsClient()
{

}

void AEvDnsClient::_ev_begin()
{
    _send_request();
}

void AEvDnsClient::_ev_finish()
{

}

void AEvDnsClient::_ev_stop()
{
    _ev_exit_signal = AEvExitSignal::close_connection;
    // close() will cancelled immediately all scync operations.
    _socket.close();
    ret_function_cb(buff.get_error(), buff.get_respond());
}

void AEvDnsClient::_ev_timeout()
{

}


void AEvDnsClient::_send_request()
{

    asio::ip::udp::resolver resolver(_ev_loop->get_io_service());
    endpoint = *resolver.resolve({asio::ip::udp::v4(), "127.0.1.1", "53"});
    buff.reset();
    if (!buff.create_dns_request(_domain, query_type))
    {
        stop();
        return;
    }
    _socket.async_send_to(asio::buffer(buff.data(), buff.size_filled()), endpoint,
                          [this](std::error_code ec, std::size_t bytes_sent)
                                  {
                                        if (ec)
                                            if (ec) {
                                                stop();
                                                return;
                                            }
                                        std::cout << "sended DNS" << std::endl;
                                        _get_respond();
                                  }
                );
}

void AEvDnsClient::_get_respond()
{
       buff.reset();
        buff.release(dns::max_DNS_pkg_size);
        _socket.async_receive_from(asio::buffer(buff.data_top(), buff.size_avail()), endpoint,
                                   [this](std::error_code ec, std::size_t bytes_sent)
                                           {
                                                 if (ec) {
                                                     return;
                                                 }
                                                 std::cout << "receive DNS" << std::endl;
                                                 buff.parse_dns_respond();
                                                 stop();
                                           }

                         );

}

void aev::AEvDnsClient::_ev_child_callback(AEvPtrBase child_ptr, AEvExitSignal &_ret)
{

}


} //namespace
