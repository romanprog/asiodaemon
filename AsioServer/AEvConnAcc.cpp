#include "AEvConnAcc.hpp"
#include "AEvConnUnit.hpp"

namespace aev {

AEvConnAcc::AEvConnAcc(AEvRootConf &config, const std::string &ip, const unsigned port)
     :AEventsAbstract::AEventsAbstract(config),
      _acceptor(_ev_loop->get_io_service()),
      _socket(_ev_loop->get_io_service()),
      _conn_ip(ip),
      _conn_port(port)
{

}

AEvConnAcc::AEvConnAcc(const AEvChildConf config, const std::string &ip, const unsigned port)
    :AEventsAbstract::AEventsAbstract(config),
     _acceptor(_ev_loop->get_io_service()),
     _socket(_ev_loop->get_io_service()),
     _conn_ip(ip),
     _conn_port(port)
{

}

void AEvConnAcc::_ev_begin()
{
    asio::error_code ec;
    asio::ip::address_v4 ip_check {asio::ip::address_v4::from_string(_conn_ip, ec)};
    if (ec || ip_check.is_unspecified()) {
        stop();
        return;
    }

    asio::ip::tcp::resolver resolver(_ev_loop->get_io_service());
    asio::ip::tcp::endpoint endpoint = *resolver.resolve({_conn_ip, std::to_string(_conn_port)});

    _acceptor.open(endpoint.protocol());
    _acceptor.set_option(asio::ip::tcp::acceptor::reuse_address(true));
    _acceptor.bind(endpoint);
    _acceptor.listen();

    AEvConnAcc::_start_acceept();

}

void AEvConnAcc::_ev_finish()
{

}

void AEvConnAcc::_ev_stop()
{

}

void AEvConnAcc::_ev_timeout()
{

}

void AEvConnAcc::_ev_child_callback(int _ret)
{

}

void AEvConnAcc::_start_acceept()
{
    using namespace std::placeholders;

    _acceptor.async_accept(_socket, _ev_loop->wrap(
                               [this] (std::error_code ec)
    {
                               if (ec)
                                    return;
                               std::cout << "conn" << std::endl;
//                               _create_child<AEvConnUnit>(_gen_conf_for_child(5), std::move(_socket));
                               _start_acceept();
                           })
            );
}

} //namespace
