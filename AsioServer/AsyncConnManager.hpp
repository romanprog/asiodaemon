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
    using StrandPtr = std::shared_ptr<asio::strand>;

    AsyncConnManager(StrandPtr strand, const std::string & ip, const unsigned port);
    void stop();
    void async_start();

private:
    // Delete copy constructor.
    AsyncConnManager(AsyncConnManager &&) = delete;
    AsyncConnManager & operator= (AsyncConnManager &&) = delete;

    SigCallback _ev_callback;
    StrandPtr _ev_loop;
    asio::deadline_timer _ev_timeout;
    bool _stoped;
    std::string _conn_ip;
    unsigned _conn_port;
    asio::ip

    void get_sig(const asio::error_code& error, int signal_number);
    void _start_sig_listel();
};

#endif // ASYNCCONNMANAGER_HPP
