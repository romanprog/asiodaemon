#include "MLocker.hpp"

void UnlockMutex(std::mutex *m)
{
    m->unlock();
}

MLocker::MLocker(std::mutex *m)
    :mutex(m, UnlockMutex)
{
    mutex->lock();
}




void MLocker::unlock()
{
    mutex->unlock();
}
