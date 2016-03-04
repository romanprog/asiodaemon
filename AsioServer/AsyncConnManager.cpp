#include "AsyncConnManager.hpp"

AsyncConnManager::AsyncConnManager(asio::io_service & io_service, SigCallback cb)
    :_ev_callback(cb),
     _timeout_ev(io_service),
     _stoped(false)
{  }

void AsyncConnManager::stop()
{
    _stoped = true;
}

void AsyncConnManager::async_start()
{
    if (_stoped)
        return;

     _start_sig_listel();
}

void AsyncConnManager::get_sig(const asio::error_code& error, int signal_number)
{
    if (error) {
        _stoped = true;
        return;
    }

    int cb_res = MainCallback(signal_number);
    if (cb_res == StopSigListening)
    {
        _stoped = true;
        return;
    }

    _start_sig_listel();
}

void AsyncConnManager::_start_sig_listel()
{
       SygSet.async_wait(std::bind(&AsyncConnManager::get_sig, this, std::placeholders::_1, std::placeholders::_2));
}
