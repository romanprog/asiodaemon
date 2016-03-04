#include "AsyncSigListen.hpp"


AsyncSigListener::AsyncSigListener(std::shared_ptr<asio::strand> strand, SygCallback cb)
    :MainCallback(cb),
     _strand(strand),
     SygSet(_strand->get_io_service()),
     stoped(false)
{  }

void AsyncSigListener::add(int s_num)
{
    SygSet.add(s_num);
}

void AsyncSigListener::stop()
{
    stoped = true;
}

void AsyncSigListener::async_start()
{
    if (stoped)
        return;

    _start_syg_listel();
}

AsyncSigListener::~AsyncSigListener()
{
    SygSet.cancel();
}

void AsyncSigListener::sig_receiver(const asio::error_code& error, int signal_number)
{
    if (error) {
        stoped = true;
        return;
    }

    int cb_res = MainCallback(signal_number);
    if (cb_res == StopSigListening)
    {
        stoped = true;
        return;
    }

    _start_syg_listel();
}

void AsyncSigListener::_start_syg_listel()
{
    auto s_w = _strand->wrap(std::bind(&AsyncSigListener::sig_receiver, this, std::placeholders::_1, std::placeholders::_2));
    SygSet.async_wait(s_w);
}
