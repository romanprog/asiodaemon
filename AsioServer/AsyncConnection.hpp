#ifndef ASYNCCONNECTION_HPP
#define ASYNCCONNECTION_HPP

#include <iostream>
#include <functional>
#include <memory>
#include <asio.hpp>
#include <string>

class AsyncConnection
{
public:
    using SigCallback = std::function<int (int)>;
    using StrandPtr = std::shared_ptr<asio::strand>;

    AsyncConnection(StrandPtr strand, asio::ip::tcp::socket socket);
    ~AsyncConnection();
    void stop();
    void async_start();

private:
    // Delete copy constructor.
    AsyncConnection(AsyncConnection &&) = delete;
    AsyncConnection & operator= (AsyncConnection &&) = delete;

    SigCallback _ev_callback;
    StrandPtr _ev_loop;
    asio::deadline_timer _ev_timeout;
    bool _stoped {false};
    bool _have_error {false};
    asio::ip::tcp::socket _socket;
    std::array<char, 8192> _buffer;
};

#endif // ASYNCCONNECTION_HPP
