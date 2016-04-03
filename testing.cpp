#include <random>
#include <iostream>
#include <functional>
#include <memory>
#include <algorithm>

// friend class
#include <iostream>
#include "AsyncEvent/BufferBase/BuffAbstract.hpp"
#include "AsyncEvent/BufferBase/ParsingBuffAbstract.hpp"
#include "HUtils/HStrings.hpp"

class TestBufer : public ParsingBuffAbstract
{
public:
    TestBufer()
        :ParsingBuffAbstract::ParsingBuffAbstract(" ", 2048)
    {}
protected:

    int tr = 100;

private:

    virtual void when_have_new_part(const size_t begin_offset, const size_t size) override
    {

    }

};


class Base
{
public:
    Base(int t)
        :data(t)
    { }
    void set_data(int d)
    {
        data = d;
    }

protected:

    int data;

};
class Der1 : public Base
{
public:
    Der1()
        :Base::Base(0)
    { }
    void print()
    {
        std::cout << data << " Derived 1" << std::endl;
    }
};

class Der2 : public Base
{
public:
    Der2()
        :Base::Base(0)
    { }
    void print()
    {
        std::cout << data << " Derived 2" << std::endl;
    }
    void print2()
    {
        std::cout << data << " Print 2 Derived 2" << std::endl;
    }

};

template <typename T, int qtp = T::classVar>
class TestBuffDer : public TestBufer
{
public:
    explicit TestBuffDer(typename T::funcType cb, std::string s)
        :callback(cb),
         bcl(std::make_unique<typename T::baseType>())
    {   }

    void cb()
    {
        bcl->set_data(qtp);
        callback(std::move(bcl));
    }

    typename T::funcType callback;
    std::unique_ptr<typename T::baseType> bcl;

};
struct MXrec
{
    using baseType = Der2;
    using retType = std::unique_ptr<Der2>&&;
    using funcType = std::function<void (retType)>;
    static const int classVar {100};
};

struct Arec
{
    using baseType = Der1;
    using retType = std::unique_ptr<Der1>&&;
    using funcType = std::function<void (retType)>;
    static constexpr int classVar {90};
};

int main () {


    auto fcb2 = [](auto&& i)
    {
        i->print2();
    };

    auto fcb1 = [](auto&& i)
    {
       i->print();
    };


    std::string str("The following three command are defined to support the sending "
                    "options.  These are used in the mail transaction instead of the "
                    "MAIL command and inform the receiver-SMTP of the special semantics"
                    "of this transaction:");

    TestBuffDer<Arec> _buf_derived(fcb1, str);

    _buf_derived.cb();

    TestBufer _buffer;
    _buffer << str;
    _buffer.reset();
    _buffer << str;

    return 0;

}

