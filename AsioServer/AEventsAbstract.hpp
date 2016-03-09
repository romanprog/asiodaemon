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
    void begin();

private:
        AEvIoPtr _asio_io;

protected:
    virtual void _ev_begin() = 0;
    virtual void _ev_finish() = 0;
    virtual void _ev_stop() = 0;
    virtual void _ev_timeout() = 0;
    virtual void _ev_child_callback(int _ret) = 0;


    template <typename EvType, typename... _Args>
    void _create_child(_Args&&... __args)
    {
         AEvPtrBase child_ev = std::make_shared<EvType>(std::forward<_Args>(__args)...);
         child_ev->begin();
         _child_ev_list.insert(child_ev);
    }
    AEvStrandPtr _ev_loop;
    int _ev_status{0};
    AEvChildConf _gen_conf_for_child(int timeout);


protected:
    AEventsAbstract(AEventsAbstract &&) = delete;
    AEventsAbstract & operator= (AEventsAbstract &&) = delete;


    void finish();
    void reset_and_start_timer();

    int _child_callback(AEvPtrBase _child, int _ret);
    void _timer_callback();

    AEvStatus _status;

    AEvFinishCallback _finish_callback;
    unsigned _timeout;
    unsigned _id;
    AEvSet _child_ev_list;
    AEvPtrBase _my_ptr {nullptr};
    AEvTimer _timer;
};

} // namespace

#endif // AEVENTSABSTRACT_HPP
