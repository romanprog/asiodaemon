#include "AEvConnUnit.hpp"

#include "iostream"

namespace aev {

AEvConnUnit::AEvConnUnit(const AEvChildConf config, asio::ip::tcp::socket _soc)
    :AEventsAbstract::AEventsAbstract(config),
     _socket(std::move(_soc))
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
    _ev_exit_signal = close_connection;
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

    _socket.async_read_some(asio::buffer(_buffer.data(), _buffer.size()),
                           _ev_loop->wrap([this](std::error_code ec, std::size_t bytes_transferred){

        if (ec) {
            return;
        }

        reset_and_start_timer();
        auto iter = _buffer.begin();

        for (int i = bytes_transferred; i > 0; --i) {
            auto ch =  static_cast<unsigned>(static_cast<unsigned char>(*iter));
            std::cout << "Char num:" << ch << std::endl;

            if (ch == 4) {
                std::cout << "Close connection:" << ch << std::endl;
                stop();
                return;
            }
            ++iter;
        }

        std::cout << std::string(_buffer.data(), bytes_transferred) << bytes_transferred << std::endl;
        _start_read();
    }));

}


} //namespace#include "AEvConnUnit.hpp"


