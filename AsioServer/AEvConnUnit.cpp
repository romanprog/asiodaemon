#include "AEvConnUnit.hpp"

namespace aev {

AEvConnUnit::AEvConnUnit(const AEvChildConf config, asio::ip::tcp::socket _soc)
    :AEventsAbstract::AEventsAbstract(config),
     _socket(std::move(_soc))
{

}

void AEvConnUnit::_ev_begin()
{
    _start_read();
}

void AEvConnUnit::_ev_finish()
{
    std::cout << "asasdf" << std::endl;
}

void AEvConnUnit::_ev_stop()
{
    _socket.cancel();

}

void AEvConnUnit::_ev_timeout()
{

}

void AEvConnUnit::_ev_child_callback(int _ret)
{

}

void AEvConnUnit::_start_read()
{
    std::cout << "accept" << std::endl;

    _socket.async_read_some(asio::buffer(_buffer.data(), _buffer.size()),
                           [this](std::error_code ec, std::size_t bytes_transferred){

        if (ec) {
            stop();
            return;
        }

        auto iter = _buffer.begin();

        for (int i = bytes_transferred; i>0;--i) {
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
    });

}


} //namespace#include "AEvConnUnit.hpp"


