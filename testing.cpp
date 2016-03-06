#include <iostream>
#include <string>
#include <set>
#include <tuple>
#include <map>
#include <memory>

class base
{
public:
    base(int _i)
        :i(_i)
    { }
    int i;
};

class d: public base
{
public:
    d()
        :base::base(1) {}
    int sd;
};

class c: public base
{
public:
    c()
        :base::base(2) {}
    int sd;
};


template <typename EvType, typename... _Args>
void _child_callback(_Args&&... __args)
{
    std::shared_ptr<base> p = std::make_shared<EvType>(std::forward<_Args>(__args)...);
    // _Args&&... __args
}

int main()
{
    d tst;
    _child_callback<d>();

}
