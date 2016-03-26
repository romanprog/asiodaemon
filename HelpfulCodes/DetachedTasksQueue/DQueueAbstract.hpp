#ifndef DQUEUEABSTRACT_HPP
#define DQUEUEABSTRACT_HPP

#include "../MLocker.hpp"

#include <mutex>
#include <thread>
#include <queue>
#include <memory>
#include <functional>

//////////////////////////////////////////////////////////////////////////////////////
/// Abstract class for create detached (in own thread) queue worker. Create interface
/// for add task and stop worker loop. Contain virtual method _do_job() wich must be
/// overrided in derived class. Actual use if you want to perform a blocking action
/// as asynchronous event.
//////////////////////////////////////////////////////////////////////////////////////

// T - type of argument that describes the job.
// F - std::function obj, wich should be called from _do_job() after job finished.
template <class T, class F>
class DQueueBase
{
    // Main queue type.
    using CallbackPair = std::pair<T, F>;
    using AQueue = std::queue<CallbackPair>;

public:
    // Init new thread for _main_worker function with itself context (this pointer).
    DQueueBase()
        :Mthread(std::thread(&DQueueBase::_main_worker, this)) { }

    // Virtual destructor for abstract class.
    virtual ~DQueueBase() { }

    // Base method for create new job. Do not override!
    // Return immediately.
    void push_task(T _t, F _f)
    {
        // If working loop is user stoped - dont accept new task.
        if (UserStoped)
            return;

        // Lock data accees.
        MLocker _lock_data(&DataLock);
        // Push new job.
        MainQueue.push(CallbackPair(_t, _f));
        // Awakene worker.
        SleepingLock.unlock();
        // Unlock data.
    }

    // Thread join() wrapper.
    void join()
    {
        Mthread.join();
    }

    // Causes the finalization of working loop. Bloked creation of new tasks.
    // When last task will be done - stop work and thread.
    void stop()
    {
        // Mark working loop as stoped.
        UserStoped = true;
        // Awakene worker for shutdown event.
        SleepingLock.unlock();
    }

protected:
    // Main jobs queue.
    AQueue MainQueue;

private:
    // Delete copy constructor.
    DQueueBase(DQueueBase &&) = delete;
    DQueueBase & operator= (DQueueBase &&) = delete;
    // Mutex for empty queue (sleaping thread).
    std::mutex SleepingLock;
    // Mutex for multithreading data accees.
    std::mutex DataLock;
    // Marker to finalize work.
    bool UserStoped{false};

    // Self thread.
    std::thread Mthread;

    // Queue loop. Process mutexes and run worker of derived class.
    void _main_worker()
    {
        while (true)
        {
            // Sleep until no tasks.
            SleepingLock.lock();
            MLocker _lock_data(&DataLock);

            if (MainQueue.empty()) {
                if (UserStoped) {
                    // Break main loop. Stop tread.
                    return;
                }
                else {
                    // Somthing strange:
                    // queue is empty, but worker was awakened. Go sleep, worker.
                   _lock_data.unlock();
                   continue;

                }
            }

            auto _first(std::move(MainQueue.front().first));
            auto _second(std::move(MainQueue.front().second));

            // _type_GN<decltype(_second)> _second_name;
            
            // Delete job.
            MainQueue.pop();

            // Unlock queue before main job.
            _lock_data.unlock();

            // Main job (in derived class).
            _do_job(_first, _second);

            // If no job - sleep.
            if (!MainQueue.empty())
                SleepingLock.unlock();
        }
    }
    // Main virtual functtion. Get T data, do job, call F function (callback).
    // Must be owerrided in derived class
    virtual void _do_job(const T & _t, F & _f) = 0;
};

#endif // DQUEUEABSTRACT_HPP
