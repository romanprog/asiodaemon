#include "AsyncQueue.hpp"
#include "MLocker.hpp"

#include <functional>
#include <iostream>
#include <string>


void AsyncQueueExample::_do_job(const std::string& _s, QueueCallbackFunction& _f)
{
    std::cout << "Job content: " << _s << std::endl;
    _f(_s);
}

