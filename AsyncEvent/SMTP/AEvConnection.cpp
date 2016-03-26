#include "AEvConnection.hpp"
#include "../DNS/AEvDnsClient.hpp"


#include "iostream"

namespace aev {

AEvConnection::AEvConnection(const AEvChildConf config, asio::ip::tcp::socket _soc)
    :AEventAbstract::AEventAbstract(config),
     _socket(std::move(_soc))
{
    // std::cout << "AEvConnection CONSTRUCTOR! " << std::endl;
}

void AEvConnection::_ev_begin()
{
    _read_buf.init_async(_gen_conf_for_util());
    _start_send();
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
    AEvExitSignal ct= _ret;
}

void AEvConnection::_start_read()
{
    _read_buf.release(80);

    _socket.async_read_some(asio::buffer(_read_buf.data_top(), _read_buf.size_avail()),
                            _ev_loop->wrap([this](std::error_code ec, std::size_t bytes_transferred){

                                if (ec) {
                                    return;
                                }
                                _read_buf.parse_smtp(bytes_transferred, std::bind(&AEvConnection::_start_send, this));

                            }));

}

void AEvConnection::_start_send()
{
    if (_read_buf.abort) {
        stop();
        return;
    }

    _socket.async_send(asio::buffer(_read_buf.answer()),
                       _ev_loop->wrap([this](std::error_code ec, std::size_t bytes_transferred){

                           if (ec) {
                               return;
                           }

                           if (_read_buf.waiting_for_command) {
                               _start_read();
                               return;

                           }
                           reset_and_start_timer();
                           _start_read();
                       }));
}

} //namespace


