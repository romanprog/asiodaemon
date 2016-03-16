#ifndef AEVSYSSIG_HPP
#define AEVSYSSIG_HPP

#include "AEventsAbstract.hpp"

namespace aev {

class AEvSysSig : public AEventsAbstract
{
public:

    AEvSysSig(const AEvChildConf config);
    virtual ~AEvSysSig() override
    {

    }

private:

    asio::signal_set _sig_set;

    virtual void _ev_begin() override;
    virtual void _ev_finish() override;
    virtual void _ev_stop() override;
    virtual void _ev_timeout() override;
    virtual void _ev_child_callback(AEvExitSignal _ret) override;

    void _sig_listen();
};

} //namespace
#endif // AEVSYSSIG_HPP
