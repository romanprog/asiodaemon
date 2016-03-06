#ifndef AEVENTSTYPS_HPP
#define AEVENTSTYPS_HPP

#include <set>
#include <map>
#include "asio.hpp"

// Asyncronus Events chain namespase
namespace aev {

class AEventsAbstract;

using AEvPtr = std::shared_ptr<AEventsAbstract>;
using AEvFinishCallback = std::function<int (AEvPtr, int)>;
using AEvStrandPtr = std::shared_ptr<asio::strand>;
using AEvSet = std::set<AEvPtr>;

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


} //namespace aev

#endif // AEVENTSTYPS_HPP
