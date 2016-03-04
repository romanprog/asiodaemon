#ifndef ASYNCCONNMANAGER_HPP
#define ASYNCCONNMANAGER_HPP


#include <iostream>
#include <functional>
#include <memory>
#include <asio.hpp>

class AsyncConnManager
{
public:
    using SigCallback = std::function<int (int)>;

    AsyncConnManager(asio::io_service &io_service, SigCallback cb);
    void stop();
    void async_start();

private:
    // Delete copy constructor.
    AsyncConnManager(AsyncConnManager &&) = delete;
    AsyncConnManager & operator= (AsyncConnManager &&) = delete;

    SigCallback _ev_callback;
    asio::deadline_timer _timeout_ev;
    bool _stoped;

    void get_sig(const asio::error_code& error, int signal_number);
    void _start_sig_listel();
};

#endif // ASYNCCONNMANAGER_HPP
