#include "AEvSysSig.hpp"

namespace aev {

AEvSysSig::AEvSysSig(const AEvChildConf config)
    :AEventsAbstract::AEventsAbstract(config),
     _sig_set(_ev_loop->get_io_service())
{

}

void AEvSysSig::_ev_begin()
{
    _sig_set.add(SIGHUP);
    _sig_set.add(SIGUSR2);
    _sig_set.add(SIGUSR1);
    _sig_set.add(SIGALRM);

}

void AEvSysSig::_ev_finish()
{

}

void AEvSysSig::_ev_stop()
{

}

void AEvSysSig::_ev_timeout()
{

}

void AEvSysSig::_ev_child_callback(int _ret)
{

}

void AEvSysSig::_sig_listen()
{
    _sig_set.async_wait(_ev_loop->wrap(
                            [this] (std::error_code ec, int signal_number)
                        {

                            _ev_status = 1;
                            stop();
                        })
            );

}


} //namespace#include "AEvConnUnit.hpp"
