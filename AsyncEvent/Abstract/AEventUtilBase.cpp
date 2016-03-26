#include "AEventUtilBase.hpp"

namespace aev {

AEventUtilBase::AEventUtilBase()
{

}

AEventUtilBase::~AEventUtilBase()
{
    _event_manager.reset();
}

void AEventUtilBase::init_async(const AEvUtilConf config)
{
    _event_manager = config.ev_manager;
}

}
