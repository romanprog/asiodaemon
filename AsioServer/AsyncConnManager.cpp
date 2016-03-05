#include "AsyncConnManager.hpp"

AsyncConnManager::AsyncConnManager(StrandPtr strand, const std::string &ip, const unsigned port)
    :_ev_callback(cb),
     _ev_loop(strand),
     _ev_timeout(strand->get_io_service()),
     _stoped(false),
     _conn_ip(ip),
     _conn_port(port)
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
