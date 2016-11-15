#include "AEvAcceptor.hpp"
#include "../SysSig/AEvSysSig.hpp"
#include "../DNS/DnsBuffer.hpp"
#include "../../HUtils/HNet.hpp"
#include "Modules/BaseMod.hpp"

#include "iostream"
#include <thread>
#include <chrono>

namespace aev {

AEvAcceptor::AEvAcceptor(AEvRootConf &config, const std::string &ip, const unsigned port)
     :AEventAbstract::AEventAbstract(config),
      _acceptor(_ev_loop->get_io_service()),
      _socket(_ev_loop->get_io_service()),
      _conn_ip(ip),
      _conn_port(port)
{
     log_debug_aev("AEvAcceptor CONSTRUCTOR! ");
}

AEvAcceptor::AEvAcceptor(AEvChildConf && config, const std::string &ip, const unsigned port)
    :AEventAbstract::AEventAbstract(std::move(config)),
     _acceptor(_ev_loop->get_io_service()),
     _socket(_ev_loop->get_io_service()),
     _conn_ip(ip),
     _conn_port(port)
{
     log_debug_aev("AEvAcceptor CONSTRUCTOR! ");
}

void AEvAcceptor::_ev_begin()
{

    if (!hnet::is_ip_v4(_conn_ip)) {
        stop();
        return;
    }

    asio::ip::tcp::resolver resolver(_ev_loop->get_io_service());
    asio::ip::tcp::endpoint endpoint = *resolver.resolve({_conn_ip, std::to_string(_conn_port)});

    _acceptor.open(endpoint.protocol());
    _acceptor.set_option(asio::ip::tcp::acceptor::reuse_address(true));
    _acceptor.bind(endpoint);
    _acceptor.listen();
    create_child<AEvSysSig>(0);
    // Init SMTP protocol handlers map.
    smtp::BaseMod::RegisterCommands(_handlers_map);
    _start_acceept();

}

void AEvAcceptor::_ev_finish()
{

}

void AEvAcceptor::_ev_stop()
{
    asio::error_code ec;
    _acceptor.cancel(ec);
    log_debug_aev("AEvConnection _ev_stop: " );
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
    _acceptor.async_accept(_socket, wrap_callback(
                               [this] (std::error_code ec)
    {
                               if (ec) {
                                   stop();
                                   return;
                               }

                               log_debug_aev("conn");

                                create_child<AEvSmtpSession>(0, std::move(_socket), _handlers_map, std::bind(&AEvAcceptor::_new_message_handler, this, std::placeholders::_1));
                               _start_acceept();
                           })
            );
}

void AEvAcceptor::_new_message_handler(SmtpStatePtr &&message_)
{
    ++_msg_counter;
    log_main("Acceepted new message. Count: " + std::to_string(_msg_counter));
}

} //namespace
