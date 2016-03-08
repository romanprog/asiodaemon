#ifndef AEVENTSTYPESTEST_HPP
#define AEVENTSTYPESTEST_HPP
#include <set>
#include <map>
#include <chrono>
#include <asio.hpp>
#include <asio/steady_timer.hpp>

// Asyncronus Events chain namespase
namespace aev {

class base;

using AEvPtrBase = std::shared_ptr<base>;
using AEvPtrBaseConst = std::shared_ptr<const base>;

using AEvFinishCallback = std::function<void (AEvPtrBase, int)>;
using AEvStrandPtr = std::shared_ptr<asio::strand>;
//using AEvTimer = asio::basic_deadline_timer<std::chrono::system_clock, asio::detail::chrono_time_traits<std::chrono::system_clock, asio::wait_traits<std::chrono::system_clock>>>;
using AEvTimer = asio::steady_timer;
using AEvSet = std::set<AEvPtrBase>;
using AEvIoPtr = std::shared_ptr<asio::io_service>;

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

struct AEvRootConf
{
    explicit AEvRootConf(AEvFinishCallback cb)
        :onFinishCallback(cb)
    {}

    AEvStrandPtr evloop = nullptr;
    AEvFinishCallback onFinishCallback;
    unsigned timeout = 0;
};


enum AEvStatus
{
    evroot,
    evchild
};

static const size_t ev_default_timecheck = 5;

} //namespace aev


#endif // AEVENTSTYPESTEST_HPP
