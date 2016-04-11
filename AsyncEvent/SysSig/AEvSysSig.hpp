#ifndef AEVSYSSIG_HPP
#define AEVSYSSIG_HPP

#include "../AEvBase/AEventAbstract.hpp"

namespace aev {

class AEvSysSig : public AEventAbstract
{
public:

    AEvSysSig(AEvChildConf && config);
    virtual ~AEvSysSig() override
    {

    }

private:

    asio::signal_set _sig_set;

    virtual void _ev_begin() override;
    virtual void _ev_finish() override;
    virtual void _ev_stop() override;
    virtual void _ev_timeout() override;
    virtual void _ev_child_callback(AEvPtrBase child_ptr, AEvExitSignal & _ret) override;

    void _sig_listen();
};

} //namespace
#endif // AEVSYSSIG_HPP
