#include <iostream>
#include <string>
#include <set>
#include <tuple>
#include <map>
#include <memory>
#include <functional>
#include <thread>
#include <asio.hpp>
#include <asio/steady_timer.hpp>

class base;

using AEvPtrBase = std::shared_ptr<base>;
using AEvPtrBaseConst = std::shared_ptr<const base>;

using AEvFinishCallback = std::function<void (AEvPtrBase, int)>;
using AEvStrandPtr = std::shared_ptr<asio::strand>;
//using AEvTimer = asio::basic_deadline_timer<std::chrono::system_clock, asio::detail::chrono_time_traits<std::chrono::system_clock, asio::wait_traits<std::chrono::system_clock>>>;
using AEvTimer = asio::steady_timer;
using AEvSet = std::set<AEvPtrBase>;
using AEvIoPtr = std::shared_ptr<asio::io_service>;

struct AEvChildConf
{
    explicit AEvChildConf(AEvStrandPtr el, AEvFinishCallback cb, unsigned t_out)
        :evloop(el),
         onFinishCallback(cb),
         timeout(t_out)
    {}

    AEvStrandPtr evloop;
    AEvFinishCallback onFinishCallback;
    unsigned timeout = 0;
};

struct AEvRootConf
{
    explicit AEvRootConf(AEvFinishCallback cb)
        :onFinishCallback(cb)
    {}

    AEvStrandPtr evloop = nullptr;
    AEvFinishCallback onFinishCallback;
    unsigned timeout = 0;
};


enum AEvStatus
{
    evroot,
    evchild
};

static const size_t ev_default_timecheck = 5;

class base;
class derived2;

class base : public std::enable_shared_from_this<base>
{
public:
    explicit base(AEvRootConf _config)
        :_parent_cb(_config.onFinishCallback),
         _io_service(std::make_shared<asio::io_service>()),
         _strand(std::make_shared<asio::strand>(*_io_service.get())),
         _timer(_strand->get_io_service())

    {
        _config.evloop = std::make_shared<asio::strand>(_strand->get_io_service());
    }
    explicit base(AEvChildConf _config)
        :_parent_cb(_config.onFinishCallback),
         _io_service(nullptr),
         _strand(std::make_shared<asio::strand>(_config.evloop->get_io_service())),
         _timer(_strand->get_io_service())

    {
        _config.evloop = std::make_shared<asio::strand>(_strand->get_io_service());
    }

    virtual ~base() {
        std::cout << "Base destructor." << std::endl;
    }

    virtual int main_work() = 0;
    virtual void on_timer() = 0;

    void start(){

        _start_timer();
        main_work();
    }

    void run()
    {
        _strand->get_io_service().run();
    }

    void _start_timer()
    {
        _timer.expires_from_now(std::chrono::seconds(1));
        _timer.async_wait([this](const asio::error_code & ec)
        {
            if (ec) {
                std::cout << "Timer Error: " << ec.message() << std::endl;
                return;
            }

            std::cout << "Timer: " << _ptrset.size() << std::endl;

            _start_timer();

            on_timer();
        });
    }

    AEvChildConf _gen_child_conf()
    {
        auto cb = std::bind(&base::_callback_for_child, this, std::placeholders::_1, std::placeholders::_2);
        AEvChildConf ret(_strand, cb, 3);
        return ret;
    }

    template <typename EvType, typename... _Args>

    void _create_child(_Args&&... __args)
    {

        AEvPtrBase p = std::make_shared<EvType>(_gen_child_conf(), std::forward<_Args>(__args)...);
        _ptrset.insert(p);
        p->start();

        std::cout << "Child created, childs count: " << _ptrset.size() << std::endl;
    }


    void _callback_for_child(AEvPtrBase _cptr, int i)
    {
        std::cout << "Child deleted, childs before: " << _ptrset.size();
        _ptrset.erase(_cptr);
        std::cout << ", childs after: " << _ptrset.size() << std::endl;
    }

    void finish()
    {
        _timer.cancel();
        _timer.cancel_one();
        std::cout << "Finishing child, try call parent callback function." << std::endl;
        auto sdf = shared_from_this();
        _parent_cb(shared_from_this(), 1);

    }

    AEvFinishCallback _parent_cb;
    std::set<AEvPtrBase> _ptrset;
    AEvIoPtr _io_service;
    AEvStrandPtr _strand;
    AEvTimer _timer;

};



class derived1: public base
{
public:
    explicit derived1(AEvRootConf _conf, std::string _name)
        :base::base(_conf),
         der_string(_name)
    {}
    explicit derived1(AEvChildConf _conf, std::string _name)
        :base::base(_conf),
          der_string(_name)
    {}
    int _tconuter {0};
    std::string der_string;

    virtual ~derived1() {
        std::cout << "Derived1 destructor." << std::endl;
    }

    virtual int main_work()
    {
        std::cout << "derived1 virtual func der_string = " <<  der_string << std::endl;
        _create_child<derived2>(1000);

        std::cout << "derived1 create child derived2" <<  der_string << std::endl;
        return 50;
    }
    virtual void on_timer()
    {
        std::cout << "derived1 timer = " << _tconuter <<  std::endl;
        ++_tconuter;
    }

};

class derived2: public base
{
public:
    explicit derived2(AEvRootConf _conf, int _num)
        :base::base(_conf),
         der_int(_num)
    {}
    explicit derived2(AEvChildConf _conf, int _num)
        :base::base(_conf),
         der_int(_num)
    {}

    virtual ~derived2() {
        std::cout << "Derived2 destructor." << std::endl;
    }

    int der_int;
    int _tconuter {0};

    void d2_test()
    {
        std::cout << "d2 test = " << std::endl;
    }

    virtual int main_work()
    {
        std::cout << "derived2 virtual func der_int = " <<  der_int << std::endl;

        std::this_thread::sleep_for(std::chrono::seconds(1));

        // _finish_child();
        return 100;
    }
    virtual void on_timer()
    {
        std::cout << "derived2 timer = " << _tconuter << std::endl;
        ++_tconuter;
        if (_tconuter == 6)
            finish();

        auto func = [this](int i)
        {
            d2_test();
        };
        func(1);

    }
};


void _callback_for_root(AEvPtrBase _cptr, int i)
{
    std::cout << "Root finised!: " << std::endl;
}

int main()
{
    AEvRootConf conf(_callback_for_root);
    derived1 tst(conf, "Root obj!");
    tst.start();
    tst.run();

}
