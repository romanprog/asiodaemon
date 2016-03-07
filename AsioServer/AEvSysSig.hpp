#ifndef AEVSYSSIG_HPP
#define AEVSYSSIG_HPP

#include "AEventsAbstract.hpp"

namespace aev {

class AEvSysSig : public AEventsAbstract
{
public:

    AEvSysSig(const AEvChildConf config);

private:

    asio::signal_set _sig_set;

    virtual void _ev_begin();
    virtual void _ev_finish();
    virtual void _ev_stop();
    virtual void _ev_timeout();
    virtual void _ev_child_callback(int _ret);

    void _sig_listen();

};

} //namespace
#endif // AEVSYSSIG_HPP
