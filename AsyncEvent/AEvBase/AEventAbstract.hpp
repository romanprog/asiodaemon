#ifndef AEVENTSABSTRACT_HPP
#define AEVENTSABSTRACT_HPP
#include "../../Logger/Logger.hpp"

#include "AEventTypes.hpp"

namespace aev {

class AEventAbstract : public std::enable_shared_from_this<AEventAbstract>
{
    friend class AEventUtilBase;

public:

    // Derived class must have 1 constructor with const AEvChildConf (in case it could not be root) or
    // 2 constructors with const AEvChildConf and AEvRootConf args. Conf args must be translated to base class.
    explicit AEventAbstract(AEvRootConf & config);
    // Child event must be created by @create_child member method. AEvChildConf generate by _gen_conf_for_child method
    // so it can be only rvalue.
    explicit AEventAbstract(AEvChildConf && config);
    virtual ~AEventAbstract();

    // Init stop event. Call _ev_stop() in derived class.
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
    // Temporary method for debugging.
    std::string debug_get_id();

    // Virtual methods.
    virtual void _ev_begin() = 0;
    virtual void _ev_finish() = 0;
    virtual void _ev_stop() = 0;
    virtual void _ev_timeout() = 0;
    virtual void _ev_child_callback(AEvPtrBase child_ptr, AEvExitSignal & _ret) = 0;

    // Create child event of any derived type.
    // Args: timeout milliseconds or 0 (without timeout), !addinional! arguments of derived type.
    template <typename AEvDescT, typename... _Args>
    void create_child(int timeout, _Args&&... __args)
    {
        static_assert(std::is_base_of<AEventAbstract, AEvDescT>::value,
                      "AEvDescT must be a descendant of AEventAbstract");

//        static_assert(std::is_constructible<AEvDescT, AEvChildConf, _Args...>::value,
//                      "Can't construct child object with given params. The "
//                      "first must be AEvChildConf! Please check constructor of descendant class.");

        AEvPtrBase child_ev = std::make_shared<AEvDescT>(_gen_conf_for_child(timeout), std::forward<_Args>(__args)...);
        _child_ev_list.insert(child_ev);
        child_ev->begin();
    }
    // Wrap handler for async events (like asio::async_read()), add pointer to AEv object, which pushed this handler in queue.
    // Return a function object that, when invoked, passes the wrapped handler to
    // the strand's dispatch function. (see asio::io_service::strand::wrap)
    template <typename T>
    auto wrap_asio_cb(T&& cb)
    {
        AEvHandlerWrapper<T> res {std::forward<T>(cb), _my_ptr};
        return _ev_loop->wrap(res);
    }
    template <typename T>
    auto wrap_cb(T&& cb)
    {
        AEvHandlerWrapper<T> res {std::forward<T>(cb), _my_ptr};
        return res;
    }

    // Registration of AEv vutil deinit function.
    void register_util(AEvUtilCloseFunc deinit_func);

    // Last step of event before destruct.
    void finish();

    // Update timeout counter.
    void reset_and_start_timer();

    // Callback function for child.
    int _child_callback(AEvPtrBase _child, AEvExitSignal _ret);

    // Generate AEvChildConf for creating child event.
    AEvChildConf _gen_conf_for_child(int timeout);

    // Generate AEvChildConf for creating child event.
    AEvUtilConf _gen_conf_for_util();

    // Main asio IO service (event loop) pointer in a strand wrapper.
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

    // Registered utils. Need to remove additional shared pointer to parent AEv object.
    AEvUtilsClosersList _registered_utils;

    // Pointer to itself. Creating by begin() method. Used for parent callback for
    // request deleting from parent _child_ev_list.
    AEvPtrBase _my_ptr {nullptr};

    // Base timer object.
    AEvTimer _timer;

    // Stop process begined. Don't call again.
    bool stop_inited {false};
};

} // namespace

#endif // AEVENTSABSTRACT_HPP
