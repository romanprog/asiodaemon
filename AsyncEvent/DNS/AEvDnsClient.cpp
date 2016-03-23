#include "AEvDnsClient.hpp"
#include "../../Buffers/DnsBuffer.hpp"

#include "iostream"

namespace aev {

AEvDnsClient::AEvDnsClient(const AEvChildConf config, std::string dom)
    :AEventAbstract::AEventAbstract(config),
     _socket(config.evloop->get_io_service(), asio::ip::udp::endpoint(asio::ip::udp::v4(), 0)),
     _domain(dom)
{
    // std::cout << "AEvConnection CONSTRUCTOR! " << std::endl;
}

void AEvDnsClient::_ev_begin()
{
    _resolve();
}

void AEvDnsClient::_ev_finish()
{

}

void AEvDnsClient::_ev_stop()
{
    _ev_exit_signal = AEvExitSignal::close_connection;
//    _socket.cancel();
    _socket.close();
}

void AEvDnsClient::_ev_timeout()
{

}


void AEvDnsClient::_resolve()
{

    asio::ip::udp::resolver resolver(_ev_loop->get_io_service());
    endpoint = *resolver.resolve({asio::ip::udp::v4(), "127.0.1.1", "53"});
    buff.create_dns_request(_domain, DnsQType::PTR);
    _socket.async_send_to(asio::buffer(buff.reqest_data_str()), endpoint,
                          [this](std::error_code ec, std::size_t bytes_sent)
                                  {
                                        if (ec)
                                            std::cout << ec.message() << std::endl;
                                        std::cout << "sended DNS" << std::endl;
                                        _get_respond();
                                  }
                );
}

void AEvDnsClient::_get_respond()
{
        buff.reset();
        buff.release(max_DNS_pkg_size);
        _socket.async_receive_from(asio::buffer(buff.data_top(), buff.size_avail()), endpoint,
                                   [this](std::error_code ec, std::size_t bytes_sent)
                                           {
                                                 if (ec)
                                                     std::cout << ec.message() << std::endl;
                                                 std::cout << "receive DNS" << std::endl;
                                                 buff.accept(bytes_sent);
                                                 buff.parse_dns_respond();
                                                 stop();
                                           }

                         );

}

void aev::AEvDnsClient::_ev_child_callback(AEvPtrBase child_ptr, AEvExitSignal &_ret)
{

}


} //namespace
