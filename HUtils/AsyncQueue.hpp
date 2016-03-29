#ifndef ASYNCQUEUE_HPP
#define ASYNCQUEUE_HPP

#include "DetachedTasksQueue/DQueueAbstract.hpp"

#include <string>

// Callback function, wich is called after task from queue done.
using QueueCallbackFunction = std::function<void (std::string)>;

class AsyncQueueExample : public DQueueBase<std::string, QueueCallbackFunction>
{
public:
    AsyncQueueExample() {}


private:
    virtual void _do_job(const std::string&, QueueCallbackFunction&);

};

#endif // ASYNCQUEUE_HPP
