#include "AEvSysSig.hpp"

namespace aev {

AEvSysSig::AEvSysSig(AEvChildConf &&config)
    :AEventAbstract::AEventAbstract(std::move(config)),
     _sig_set(_ev_loop->get_io_service())
{

}

void AEvSysSig::_ev_begin()
{
    _sig_set.add(SIGHUP);
    _sig_set.add(SIGUSR2);
    _sig_set.add(SIGUSR1);
    _sig_set.add(SIGALRM);
    _sig_listen();

}

void AEvSysSig::_ev_finish()
{

}

void AEvSysSig::_ev_stop()
{
    _ev_exit_signal = AEvExitSignal::abort;
}

void AEvSysSig::_ev_timeout()
{

}

void AEvSysSig::_ev_child_callback(AEvPtrBase child_ptr, AEvExitSignal &_ret)
{

}

void AEvSysSig::_sig_listen()
{
    _sig_set.async_wait(wrap_asio_cb(
                            [this] (std::error_code ec, int signal_number)
                        {
                            stop();
                        })
            );

}


} //namespace#include "AEvConnection.hpp"
