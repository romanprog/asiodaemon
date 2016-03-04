#ifndef ASYNCSIGLISTEN_HPP
#define ASYNCSIGLISTEN_HPP

#include <iostream>
#include <functional>
#include <memory>
#include <asio.hpp>

class AsyncSigListener
{
public:
    using SygCallback = std::function<int (int)>;

    AsyncSigListener(std::shared_ptr<asio::strand> strand, SygCallback cb);
    void add(int s_num);
    void stop();
    void async_start();

    static const int StopSigListening = 1;

    ~AsyncSigListener();

private:
    // Delete copy constructor.
    AsyncSigListener(AsyncSigListener &&) = delete;
    AsyncSigListener & operator= (AsyncSigListener &&) = delete;
    SygCallback MainCallback;
    std::shared_ptr<asio::strand> _strand;
    asio::signal_set SygSet;
    bool stoped;

    void sig_receiver(const asio::error_code& error, int signal_number);
    void _start_syg_listel();
};

#endif // ASYNCSIGLISTEN_HPP
