#ifndef AEVENTSTYPS_HPP
#define AEVENTSTYPS_HPP

#include <set>
#include <map>
#include <chrono>
#include <asio.hpp>
#include <unordered_map>
#include <asio/steady_timer.hpp>
#include <functional>

// Asyncronus Events namespace
namespace aev {

class AEventAbstract;
class AEventUtilBase;

enum class AEvExitSignal
{
    // Standart exit signal, parent must remove this child from list.
    normal = 0,
    // Parent must run stop procedure for itself and send "normal" exit signal.
    stop_parent,
    // All non-root parent events must init stop proc.
    stop_branch,
    // Abort process.
    abort,
    // User signals.
    user1,
    user2,
    user3,
    // Signal from connection to acceptor.
    close_connection,
    // Sigs from "System Signals Listener".
    sys_sig_restart,
    sys_sig_stop,
    sys_sig_reload_config

};

// Event status: tp event (have no parent) - evroot. Every event, which was created by function "create_chid" has status evchild.
enum class AEvStatus
{
    evroot,
    evchild,
    evutil,
    evmodule
};

using AEvPtrBase = std::shared_ptr<AEventAbstract>;
using AEvPtrBaseConst = std::shared_ptr<const AEventAbstract>;
using AEvUtilPtr = std::shared_ptr<AEventUtilBase>;
using AEvUtilCloseFunc = std::function<void()>;

using AEvFinishCallback = std::function<int (AEvPtrBase, AEvExitSignal)>;
using AEvStrandPtr = std::shared_ptr<asio::io_service::strand>;
using AEvTimer = asio::steady_timer;
using AEvSet = std::set<AEvPtrBase>;
using AEvIoPtr = std::shared_ptr<asio::io_service>;
using AEvUtilsClosersList = std::vector<AEvUtilCloseFunc>;

template <typename F>
class AEvHandlerWrapper
{
public:
    explicit  AEvHandlerWrapper(F &&fn_, AEvPtrBase ptr_)
        : __functor(std::forward<F>(fn_)),
          __aev_ptr(std::move(ptr_))
    {}

    template <typename... ARGS>
    auto operator ()(ARGS&&... args)
    {
        return __functor(std::forward<ARGS>(args)...);
    }

private:
    F __functor;
    AEvPtrBase __aev_ptr;
};

// Config for base AEv type child (wich was created by "create_child").
struct AEvChildConf
{
    explicit AEvChildConf(AEvStrandPtr el, AEvFinishCallback cb, unsigned t_out)
        :evloop(el),
         onFinishCallback(cb),
         timeout(t_out)
    {}

    AEvStrandPtr evloop;
    AEvFinishCallback onFinishCallback;
    unsigned timeout = 0;
};

// Config for base AEv type root. Top parent.
struct AEvRootConf
{
    explicit AEvRootConf(AEvFinishCallback cb)
        :onFinishCallback(cb)
    {}
    explicit AEvRootConf(AEvFinishCallback cb, unsigned tout)
        :onFinishCallback(cb),
         timeout(tout)
    {}
    AEvStrandPtr evloop = nullptr;
    AEvFinishCallback onFinishCallback;
    unsigned timeout = 0;
};

// Config for AEv utils.
struct AEvUtilConf
{
    explicit AEvUtilConf(AEvPtrBase el)
        :ev_manager(el)

    { }
    const AEvPtrBase ev_manager;
};

// Default time in seconds for run timer event. (If event config created with 0 timeout).
static const size_t ev_default_timecheck = 5;

} //namespace aev

#endif // AEVENTSTYPS_HPP
