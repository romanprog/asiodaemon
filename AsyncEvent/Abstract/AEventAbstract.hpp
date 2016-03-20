#ifndef AEVENTSABSTRACT_HPP
#define AEVENTSABSTRACT_HPP

#include "AEventTypes.hpp"

namespace aev {

class AEventAbstract : public std::enable_shared_from_this<AEventAbstract>
{
public:
    // Derived class must have 1 constructor with const AEvChildConf (in case it could not be root) or
    // 2 constructors with const AEvChildConf and AEvRootConf args. Conf args must be translated to base class.
    explicit AEventAbstract(AEvRootConf & config);
    explicit AEventAbstract(const AEvChildConf & config);
    virtual ~AEventAbstract();
    // Init stop event event. Call _ev_stop() in derived class.
    void stop();
    // Run event loop. Call only from root object.
    void run();
    // Begin created event. Call _ev_begin() in derived class.
    void begin();

private:
    // Base IO service. Createing in root event for generate AEvRootConf.
    AEvIoPtr _asio_io;
    // Delete copy and move constructors and assignment operator.
    AEventAbstract(AEventAbstract &&) = delete;
    AEventAbstract & operator= (AEventAbstract &&) = delete;

protected:
    // Virtual methods.
    virtual void _ev_begin() = 0;
    virtual void _ev_finish() = 0;
    virtual void _ev_stop() = 0;
    virtual void _ev_timeout() = 0;
    virtual void _ev_child_callback(AEvPtrBase child_ptr, AEvExitSignal & _ret) = 0;

    // Create child event of any derived type.
    // Args: timeout seconds or 0 (without timeout), !addinional! arguments of derived type.
    template <typename EvType, typename... _Args>
    void _create_child(int timeout, _Args&&... __args)
    {
         AEvPtrBase child_ev = std::make_shared<EvType>(_gen_conf_for_child(timeout), std::forward<_Args>(__args)...);
         child_ev->begin();
         _child_ev_list.insert(child_ev);
    }

    // Last step of event before destruct.
    void finish();
    // Update timeout counter.
    void reset_and_start_timer();
    // Callback function for child.
    int _child_callback(AEvPtrBase _child, AEvExitSignal _ret);

    // Generate AEvChildConf for creating child event.
    AEvChildConf _gen_conf_for_child(int timeout);
    // Main asio IO service (event loop) pointer.
    AEvStrandPtr _ev_loop;

    // When event is finished - run parent callback function with this sinnal.
    AEvExitSignal _ev_exit_signal{AEvExitSignal::normal};
    // Status of event. See AEvStatus description.
    AEvStatus _status;
    // _finish_callback called (with _ev_exit_signal argument) when event finishing.
    AEvFinishCallback _finish_callback;
    // Timeout of event in seconds. 0 - no timeout.
    // When _timeout is 0 - every ev_default_timecheck seconds calling reset_and_start_timer.
    unsigned _timeout;
    // List of child events.
    AEvSet _child_ev_list;
    // Pointer to itself. Creating by begin() method. Used for parent callback for
    // request deleting from parent _child_ev_list.
    AEvPtrBase _my_ptr {nullptr};
    // Base timer object.
    AEvTimer _timer;
};

} // namespace

#endif // AEVENTSABSTRACT_HPP
