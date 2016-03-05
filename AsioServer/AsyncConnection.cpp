#include "AsyncConnection.hpp"
#include <tuple>


AsyncConnection::AsyncConnection(AsyncConnection::StrandPtr strand, asio::ip::tcp::socket socket)
    :_ev_loop(strand),
     _ev_timeout(strand->get_io_service()),
     _stoped(false),
     _socket(std::move(socket))
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
            std::cout << "erro" << ec.message() << std::endl;
            return;
        }

        std::cout << std::string(_buffer.data(), _buffer.size()) << bytes_transferred << std::endl;
        async_start();
});
}
