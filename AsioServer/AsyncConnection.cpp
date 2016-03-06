#include "AsyncConnection.hpp"
#include <tuple>


AsyncConnection::AsyncConnection(AsyncConnection::StrandPtr strand, asio::ip::tcp::socket socket, int chID, EvCallback cb)
    :_ev_callback(cb),
     _ev_loop(strand),
     _ev_timeout(strand->get_io_service()),
     _stoped(false),
     _socket(std::move(socket)),
     myID(chID)
{
    async_start();
}

AsyncConnection::~AsyncConnection()
{
    std::cout << "destruct" << std::endl;
}

void AsyncConnection::stop()
{
    _stoped = true;
}

void AsyncConnection::async_start()
{
    std::cout << "accept" << std::endl;

    _socket.async_read_some(asio::buffer(_buffer.data(), _buffer.size()),
                           [this](std::error_code ec, std::size_t bytes_transferred){

        if (ec) {
            _ev_callback(myID, 0);
            std::cout << "erro" << ec.message() << std::endl;
            return;
        }

        auto iter = _buffer.begin();

        for (int i = bytes_transferred; i>0;--i) {
            auto ch =  static_cast<unsigned>(static_cast<unsigned char>(*iter));
            std::cout << "Char num:" << ch << std::endl;

            if (ch == 4) {
                _ev_callback(myID, 0);
                std::cout << "Close connection:" << ch << std::endl;
                return;
            }
            ++iter;
        }

        std::cout << std::string(_buffer.data(), bytes_transferred) << bytes_transferred << std::endl;
        async_start();
    });
}

int AsyncConnection::child_callback(int chID, int command)
{
;

    return 0;
}
