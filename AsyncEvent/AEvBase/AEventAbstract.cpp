#include "AEventAbstract.hpp"
#include "AEventUtilBase.hpp"


#include <memory>
#include <chrono>
#include <iostream>

namespace aev {

AEventAbstract::AEventAbstract(aev::AEvRootConf & config)
    :_asio_io(std::make_shared<asio::io_service>()),
      _ev_loop(std::make_shared<asio::io_service::strand>(*_asio_io)),
      _status(AEvStatus::evroot),
      _finish_callback(config.onFinishCallback),
      _timeout(config.timeout),
      _timer(_ev_loop->get_io_service())
{
    // log_debug_aev("AEventsAbstract CONSTRUCTOR! ");
    config.evloop = _ev_loop;
}

AEventAbstract::AEventAbstract(AEvChildConf &&config)
    :_ev_loop(std::move(config.evloop)),
      _status(AEvStatus::evchild),
      _finish_callback(config.onFinishCallback),
      _timeout(config.timeout),
      _timer(_ev_loop->get_io_service())
{
    //log_debug_aev("AEventsAbstract CONSTRUCTOR!");
}

AEventAbstract::~AEventAbstract()
{
    //log_debug_aev("AEventsAbstract DESTRUCTOR!");
}

void AEventAbstract::begin()
{
    _my_ptr = shared_from_this();
    reset_and_start_timer();
    //log_debug_aev( "Try call _ev_begin " );
    _ev_begin();
}

std::string AEventAbstract::debug_get_id()
{
    const void * address = static_cast<const void*>(this);
    std::stringstream ss;
    ss << address;
    return ss.str();
}

void AEventAbstract::register_util(AEvUtilCloseFunc deinit_func)
{
    _registered_utils.push_back(deinit_func);
}

void AEventAbstract::finish()
{
      _timer.cancel();
    //log_debug_aev("Try call _ev_finish ");
    _ev_finish();
    _finish_callback(std::move(_my_ptr), _ev_exit_signal);
}

void AEventAbstract::reset_and_start_timer()
{
    _timer.expires_from_now(!_timeout ? std::chrono::milliseconds(ev_default_timecheck) :  std::chrono::milliseconds(_timeout));
    auto self_ptr = _my_ptr;
    _timer.async_wait(wrap_asio_cb(
                          [this, self_ptr](const asio::error_code & ec)
    {
        if (ec)
            return;

        if (!_timeout)
        {
            int todo; // Add call virtual timecheck method in case of 0 timeout.
            _timer.expires_from_now(std::chrono::milliseconds(ev_default_timecheck));
            reset_and_start_timer();
        } else {
            // timeout!
            log_debug_aev("Try call _ev_timeout ");
            _ev_timeout();
            stop();
        }
    }
    ));
}

void AEventAbstract::stop()
{
    if (stop_inited)
        return;
    stop_inited = true;

    for (auto & util_stop : _registered_utils)
        util_stop();

    auto ch_list_copy_tmp = _child_ev_list;

    for (auto & child : ch_list_copy_tmp)
        child->stop();

    // log_debug_aev("Try call _ev_stop ");

    _ev_stop();
    finish();
    return;
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

AEvUtilConf AEventAbstract::_gen_conf_for_util()
{
    return AEvUtilConf(_my_ptr);
}


int AEventAbstract::_child_callback(AEvPtrBase _child, AEvExitSignal _ret)
{
    _child_ev_list.erase(_child);
    // log_debug_aev("Try call _ev_child_callback ");
    _ev_child_callback(_child, _ret);

    return 0;
}

} //namespace aev
