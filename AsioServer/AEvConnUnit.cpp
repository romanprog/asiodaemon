#include "AEvConnUnit.hpp"

#include "iostream"

namespace aev {

AEvConnUnit::AEvConnUnit(const AEvChildConf config, asio::ip::tcp::socket _soc)
    :AEventsAbstract::AEventsAbstract(config),
     _socket(std::move(_soc)),
   _read_buf(30, "\n")
{
    std::cout << "AEvConnUnit CONSTRUCTOR! " << std::endl;
}

void AEvConnUnit::_ev_begin()
{
    _start_read();
}

void AEvConnUnit::_ev_finish()
{

}

void AEvConnUnit::_ev_stop()
{
    _ev_exit_signal = AEvExitSignal::close_connection;
    _socket.cancel();
    _socket.close();
}

void AEvConnUnit::_ev_timeout()
{

}

void AEvConnUnit::_ev_child_callback(AEvExitSignal _ret)
{

}

void AEvConnUnit::_start_read()
{
    std::cout << "accept" << std::endl;
    auto ppp = shared_from_this();
    _read_buf.release(40);
    _socket.async_read_some(asio::buffer(_read_buf.data_top(), _read_buf.free_sz()),
                           _ev_loop->wrap([this](std::error_code ec, std::size_t bytes_transferred){

        if (ec) {
            return;
        }
        std::cout << "read" << std::endl;
        if (_read_buf.parse(bytes_transferred)) {
            std::cout << _read_buf.get();

            _read_buf.reset();
        }
        reset_and_start_timer();

        _start_read();
    }));

}


} //namespace#include "AEvConnUnit.hpp"


