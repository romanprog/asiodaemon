#ifndef AEVENTSABSTRACT_HPP
#define AEVENTSABSTRACT_HPP

#include "AEventsTypes.hpp"

namespace aev {

class AEventsAbstract : public std::enable_shared_from_this<AEventsAbstract>
{
public:
    AEventsAbstract(AEvRootConf & config);
    AEventsAbstract(const AEvChildConf config);
    virtual ~AEventsAbstract();
    void stop();
    void run();

private:
    AEventsAbstract(AEventsAbstract &&) = delete;
    AEventsAbstract & operator= (AEventsAbstract &&) = delete;

    void begin();

    void finish();

    template <typename EvType, typename... _Args>
    void _create_child(_Args&&... __args)
    {
         AEvPtr child_ev = std::make_shared<EvType>(std::forward<_Args>(__args)...);
         _child_ev_list.insert(child_ev);
    }

    AEvChildConf _gen_child_conf(int timeout);
    int _child_callback(AEvPtr _child, int _ret);

    virtual void _ev_begin() = 0;
    virtual void _ev_finish() = 0;
    virtual void _ev_stop() = 0;

    AEvStatus _status;
    AEvStrandPtr _ev_loop;
    AEvFinishCallback _finish_callback;
    unsigned _timeout;
    unsigned _id;
    AEvSet _child_ev_list;
};

} // namespace

#endif // AEVENTSABSTRACT_HPP
