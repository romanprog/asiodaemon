#ifndef MLOCKER_HPP
#define MLOCKER_HPP

#include <mutex>
#include <memory>

using MutexPtr = std::shared_ptr<std::mutex>;

class MLocker
{
public:
    explicit MLocker(std::mutex *m);
    MLocker(MLocker &&) = delete;
    MLocker & operator = (MLocker &&) = delete;
    void unlock();

private:

    MutexPtr mutex;
    void munlock(std::mutex *m);
};

#endif // MLOCKER_HPP
