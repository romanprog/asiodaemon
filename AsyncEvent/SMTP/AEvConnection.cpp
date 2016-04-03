#include "AEvConnection.hpp"
#include "../DNS/AEvDnsClient.hpp"
#include <functional>


#include "iostream"

namespace aev {

AEvConnection::AEvConnection(const AEvChildConf config, asio::ip::tcp::socket _soc)
    :AEventAbstract::AEventAbstract(config),
     _socket(std::move(_soc)),
     session(std::bind(&AEvConnection::_respond_handler, this, std::placeholders::_1))
{
    // std::cout << "AEvConnection CONSTRUCTOR! " << std::endl;
}

void AEvConnection::_ev_begin()
{
    session.init_async(_gen_conf_for_util());
    session.begin(_socket.remote_endpoint().address().to_string());
    _start_read();
}

void AEvConnection::_ev_finish()
{

}

void AEvConnection::_ev_stop()
{
    _ev_exit_signal = AEvExitSignal::close_connection;
    _socket.cancel();
    _socket.close();
}

void AEvConnection::_ev_timeout()
{

}

void AEvConnection::_ev_child_callback(AEvPtrBase child_ptr, AEvExitSignal &_ret)
{

}

void AEvConnection::_start_read()
{
    read_buffer.release(1024);

    _socket.async_read_some(asio::buffer(read_buffer.data_top(), read_buffer.size_avail()),
                            _ev_loop->wrap([this](std::error_code ec, std::size_t bytes_transferred){

                                if (ec) {
                                    return;
                                }
                                read_buffer.accept(bytes_transferred);
                                if (!read_buffer.is_empty()) {
                                    session.transaction(read_buffer);
                                    read_buffer.mem_reduce();
                                }
                                 _start_read();
                            }));

}



void AEvConnection::_respond_handler(std::string data)
{

    _socket.async_send(asio::buffer(data),
                       _ev_loop->wrap([this](std::error_code ec, std::size_t bytes_transferred){

                           if (ec) {
                               stop();
                               return;
                           }
                           if (session.close_demand()) {
                               stop();
                               return;
                           }

                           reset_and_start_timer();
                       }));
}

} //namespace


