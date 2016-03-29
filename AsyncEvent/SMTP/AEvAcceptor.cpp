#include "AEvAcceptor.hpp"
#include "AEvConnection.hpp"
#include "../SysSig/AEvSysSig.hpp"

#include "iostream"

namespace aev {

AEvAcceptor::AEvAcceptor(AEvRootConf &config, const std::string &ip, const unsigned port)
     :AEventAbstract::AEventAbstract(config),
      _acceptor(_ev_loop->get_io_service()),
      _socket(_ev_loop->get_io_service()),
      _conn_ip(ip),
      _conn_port(port)
{
    // std::cout << "AEvAcceptor CONSTRUCTOR! " << std::endl;
}

AEvAcceptor::AEvAcceptor(const AEvChildConf config, const std::string &ip, const unsigned port)
    :AEventAbstract::AEventAbstract(config),
     _acceptor(_ev_loop->get_io_service()),
     _socket(_ev_loop->get_io_service()),
     _conn_ip(ip),
     _conn_port(port)
{
    // std::cout << "AEvAcceptor CONSTRUCTOR! " << std::endl;
}

void AEvAcceptor::_ev_begin()
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

    _create_child<AEvSysSig>(0);

    _start_acceept();

}

void AEvAcceptor::_ev_finish()
{

}

void AEvAcceptor::_ev_stop()
{
    _acceptor.cancel();
    std::cout << "AEvConnection _ev_stop: "  << std::endl;
}

void AEvAcceptor::_ev_timeout()
{

}

void AEvAcceptor::_ev_child_callback(AEvPtrBase child_ptr, AEvExitSignal &_ret)
{
        if (_ret == AEvExitSignal::abort)
            stop();
}


void AEvAcceptor::_start_acceept()
{
    using namespace std::placeholders;
    _acceptor.async_accept(_socket, _ev_loop->wrap(
                               [this] (std::error_code ec)
    {
                               if (ec)
                                    return;

                               std::cout << "conn" << std::endl;
                                _create_child<AEvConnection>(0, std::move(_socket));
                               _start_acceept();
                           })
            );
}

} //namespace
