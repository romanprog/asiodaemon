#ifndef AEVENTUTILBASE_HPP
#define AEVENTUTILBASE_HPP

#include "AEventAbstract.hpp"
#include "AEventTypes.hpp"

namespace aev {


class AEventUtilBase : public std::enable_shared_from_this<AEventUtilBase>
{
public:
    explicit AEventUtilBase();
    virtual ~AEventUtilBase();
    void init_async(const AEvUtilConf config);
    void stop_async();

protected:
    template <typename EvType, typename... _Args>
    bool _create_child(int timeout, _Args&&... __args)
    {
        if (_event_manager == nullptr)
            return false;

        _event_manager->create_child<EvType>(timeout, std::forward<_Args>(__args)...);

        return true;
    }

private:
    AEvPtrBase _event_manager {nullptr};

protected:


};

} // namespace aev
#endif // AEVENTUTILBASE_HPP
