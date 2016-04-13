#include "AEventUtilBase.hpp"

namespace aev {

AEventUtilBase::AEventUtilBase()
{

}

AEventUtilBase::~AEventUtilBase()
{
}

void AEventUtilBase::init_async(const AEvUtilConf config)
{
    _event_manager = config.ev_manager;
    _event_manager->register_util(std::bind(&AEventUtilBase::stop_async, this));
}

void AEventUtilBase::stop_async()
{
    _event_manager.reset();
}

}
