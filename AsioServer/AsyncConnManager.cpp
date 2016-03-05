#include "AsyncConnManager.hpp"

AsyncConnManager::AsyncConnManager(StrandPtr strand, const std::string &ip, const unsigned port)
    :_ev_loop(strand),
     _ev_timeout(strand->get_io_service()),
     _stoped(false),
     _conn_ip(ip),
     _conn_port(port),
     _socket(strand->get_io_service()),
     _acceptor(strand->get_io_service())
{
    asio::error_code ec;
    asio::ip::address_v4 ip_check {asio::ip::address_v4::from_string(_conn_ip, ec)};
    if (ec || ip_check.is_unspecified()) {
        _have_error = true;
        _stoped = true;
    }
    async_start();
}


void AsyncConnManager::stop()
{
    _stoped = true;
}

void AsyncConnManager::async_start()
{
    if (_stoped)
        return;

    asio::ip::tcp::resolver resolver(_ev_loop->get_io_service());
    asio::ip::tcp::endpoint endpoint = *resolver.resolve({_conn_ip, std::to_string(_conn_port)});

    _acceptor.open(endpoint.protocol());
    _acceptor.set_option(asio::ip::tcp::acceptor::reuse_address(true));
    _acceptor.bind(endpoint);
    _acceptor.listen();

    start_accept_();

}

void AsyncConnManager::start_accept_()
{
    _acceptor.async_accept(_socket,
                           [this] (std::error_code ec)
                           {
                             std::shared_ptr<AsyncConnection> conn_ptr = std::make_shared<AsyncConnection>(_ev_loop, std::move(_socket));
                             std::cout << "conn" << std::endl;
                             _connections.push_back(conn_ptr);
                             start_accept_();
                           }
                           );
}
