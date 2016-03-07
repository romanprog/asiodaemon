#include <iostream>
#include <string>
#include <set>
#include <tuple>
#include <map>
#include <memory>

class base;

std::set<std::shared_ptr<base>> ptrset;

class base : public std::enable_shared_from_this<base>
{
public:
    base(int _i)
        :i(_i)
    { }
    virtual int vt1() = 0;
    int i;
    void del_me()
    {
        // ptrset.erase(shared_from_this());
        std::cout << "set size" << ptrset.size() << std::endl;
        std::shared_ptr<base> t = shared_from_this();

        vt1();
    }

};

class d: public base
{
public:
    d()
        :base::base(1) {}
    int sd;
    virtual int vt1()
    {
        std::cout << "set size" << ptrset.size() << std::endl;
        return 50;
    }


};

class c: public base
{
public:
    c()

        :base::base(2) {}
    int sd;
    virtual int vt1()
    {
        return 100;
    }
};


template <typename EvType, typename... _Args>
void _child_callback(_Args&&... __args)
{
    std::shared_ptr<base> p = std::make_shared<EvType>(std::forward<_Args>(__args)...);
    ptrset.insert(p);
    std::cout << "set size" << ptrset.size() << std::endl;

    p->del_me();
    // _Args&&... __args
}

int main()
{

    d tst;
    _child_callback<d>();



}
