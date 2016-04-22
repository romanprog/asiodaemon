#ifndef REDISTYPES_HPP
#define REDISTYPES_HPP

#include <string>
#include <vector>
#include <memory>
#include <queue>
#include <functional>
#include <mutex>

namespace redis {

enum class RespType
{
    simple_str,
    error_str,
    integer,
    bulk_str,
    array,
    empty
};

struct RespData
{
    RespType type {RespType::empty};
    std::string sres;
    int ires {0};
    std::vector<RespData> ares;
    bool isnull {false};
    void reset()
    {
        ares.clear();
        ires = 0;
        sres.clear();
        isnull = false;
        type = RespType::empty;
    }
};

using RespDataPtr = std::unique_ptr<RespData>;
using RedisCallback = std::function<void (int, const RespData &)>;
using RedisCallbackQueue = std::queue<RedisCallback>;

template <typename T>
class FnHandlerQueue
{
    using FnType = std::function<T>;
public:
    FnHandlerQueue()
    {

    }
    void push(FnType cb)
    {
        std::lock_guard<std::mutex> lm(_rw_lock);
        if (!cb)
            return;

        _func_list.push(cb);
    }

    template <typename... _Args>
    bool call_and_pop_all(_Args&&... __args)
    {
        std::lock_guard<std::mutex> lm(_rw_lock);

        while (!_func_list.empty()) {
            _func_list.front()(std::forward<_Args>(__args)...);
            _func_list.pop();
        }

        return true;
    }
    template <typename... _Args>
    bool call_and_pop(_Args&&... __args)
    {
        std::lock_guard<std::mutex> lm(_rw_lock);

        if (_func_list.empty())
                return false;

        _func_list.front()(std::forward<_Args>(__args)...);
        _func_list.pop();

        return true;
    }

    void clear()
    {
        std::lock_guard<std::mutex> lm(_rw_lock);

        while (!_func_list.empty())
            _func_list.pop();

    }
    bool empty()
    {
        std::lock_guard<std::mutex> lm(_rw_lock);
        return _func_list.empty();
    }

private:

    FnType t;
    std::mutex _rw_lock;
    std::queue<FnType> _func_list;

};

} // namespace redis

#endif // REDISTYPES_HPP
