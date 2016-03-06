#include "AEventsAbstract.hpp"

#include <memory>

namespace aev {

AEventsAbstract::AEventsAbstract(aev::AEvRootConf &config)
    :_status(evroot),
     _ev_loop(std::make_shared<asio::strand>(asio::io_service())),
     _finish_callback(config.onFinishCallback),
     _timeout(config.timeout)
{
    config.evloop = std::make_shared<asio::strand>(_ev_loop->get_io_service());
}

AEventsAbstract::AEventsAbstract(const AEvChildConf config)
    :_status(evchild),
     _ev_loop(std::make_shared<asio::strand>(config.evloop->get_io_service())),
     _finish_callback(config.onFinishCallback),
     _timeout(config.timeout)
{ }

AEventsAbstract::~AEventsAbstract()
{

}

void AEventsAbstract::begin()
{
    _ev_begin();
}

void AEventsAbstract::finish()
{
    _finish_callback(shared_from_this(), 0);
    _ev_finish();
}

void AEventsAbstract::stop()
{
    for (auto child : _child_ev_list)
        child->stop();

    _ev_stop();
    finish();
}

void AEventsAbstract::run()
{
    _ev_loop->get_io_service().run();
}

AEvChildConf AEventsAbstract::_gen_child_conf(int timeout)
{
    auto cb = std::bind(&AEventsAbstract::_child_callback, this, std::placeholders::_1, std::placeholders::_2);
    return AEvChildConf(_ev_loop, std::move(cb), timeout);

}

int AEventsAbstract::_child_callback(AEvPtr _child, int _ret)
{
    if (!_ret)
        _child_ev_list.erase(_child);
    int todo;
    // child sygnals processing
    return 0;
}

} //namespace aev
