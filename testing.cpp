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
#include "AsioServer/AEvBuffer.hpp"

void test(size_t sz)
{
    std::cout << sz;
}

int main()
{


    aev::AEvBuffer buf(64, " ");
    std::cout << buf.size() << " " << buf.reserved() << " " << buf.free_sz() << std::endl;
        buf.release(300);

    std::cout << buf.size() << " " << buf.reserved() << " " << buf.free_sz() << std::endl;


    buf.read_string_test("fi sec ");
    std::cout << buf.size() << " " << buf.reserved() << " " << buf.free_sz() << std::endl;
    buf.read_string_test("33 44 55 sdf sdfsdfsdfsdf sdfsdfsdfsdf stet e5ye5y5y45y w4t34tert ");
    std::cout << buf.size() << " " << buf.reserved() << " " << buf.free_sz() << std::endl;

    buf.read_string_test("5 666666 fsdffsdlkfnkjsdhfkjsfdhgkjsdfhgkjshdfkghsd ");
    std::cout << buf.size() << " " << buf.reserved() << " " << buf.free_sz() << std::endl;

    for (std::string line = buf.get() ; !line.empty() ; line = buf.get()) {
        std::cout << "'" << line << "'" << std::endl;
    }
}
