#include "AEventAbstract.hpp"

#include <memory>
#include <chrono>

namespace aev {

AEventAbstract::AEventAbstract(aev::AEvRootConf & config)
    :_asio_io(std::make_shared<asio::io_service>()),
      _ev_loop(std::make_shared<asio::strand>(*_asio_io)),
      _status(AEvStatus::evroot),
      _finish_callback(config.onFinishCallback),
      _timeout(config.timeout),
      _timer(_ev_loop->get_io_service())
{
//    std::cout << "AEventsAbstract CONSTRUCTOR! " << std::endl;
    config.evloop = std::make_shared<asio::strand>(_ev_loop->get_io_service());
}

AEventAbstract::AEventAbstract(const AEvChildConf & config)
    :_ev_loop(std::make_shared<asio::strand>(config.evloop->get_io_service())),
      _status(AEvStatus::evchild),
      _finish_callback(config.onFinishCallback),
      _timeout(config.timeout),
      _timer(_ev_loop->get_io_service())
{
//    std::cout << "AEventsAbstract CONSTRUCTOR! " << std::endl;
}

AEventAbstract::~AEventAbstract()
{
//    std::cout << "AEventsAbstract DESTRUCTOR! " << std::endl;
}

void AEventAbstract::begin()
{
    _my_ptr = shared_from_this();
    reset_and_start_timer();
    _ev_begin();
}

void AEventAbstract::finish()
{
    _timer.cancel();
    _finish_callback(std::move(_my_ptr), _ev_exit_signal);
}

void AEventAbstract::reset_and_start_timer()
{
    _timer.expires_from_now(!_timeout ? std::chrono::seconds(ev_default_timecheck) :  std::chrono::seconds(_timeout));
    _timer.async_wait(_ev_loop->wrap(
                          [this](const asio::error_code & ec)
    {
        if (ec)
            return;

        if (!_timeout)
        {
            int todo; // Add call virtul timecheck method in case of 0 timeout.
            _timer.expires_from_now(std::chrono::seconds(ev_default_timecheck));
            reset_and_start_timer();
        } else {
            // timeout!
            _ev_timeout();
            stop();
        }
    }
    ));
}

void AEventAbstract::stop()
{
    for (auto child : _child_ev_list)
        child->stop();

    _ev_stop();
    finish();
}

void AEventAbstract::run()
{
    _ev_loop->get_io_service().run();
}

AEvChildConf AEventAbstract::_gen_conf_for_child(int timeout)
{
    auto cb = std::bind(&AEventAbstract::_child_callback, this, std::placeholders::_1, std::placeholders::_2);
    return AEvChildConf(_ev_loop, cb, timeout);
}

int AEventAbstract::_child_callback(AEvPtrBase _child, AEvExitSignal _ret)
{
    _child_ev_list.erase(_child);
    _ev_child_callback(_child, _ret);

    return 0;
}

} //namespace aev
