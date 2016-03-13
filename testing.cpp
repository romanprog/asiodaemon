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
    size_t step {32};
    size_t size {32};
    // test((size/step+1)*step);
    AEvBuffer buf(400);
    std::cout << buf.size() << " " << buf.reserved() << std::endl;
    buf.expand(3000);
    std::cout << buf.size() << " " << buf.reserved() << std::endl;
    buf.expand(3000);
    std::cout << buf.size() << " " << buf.reserved() << std::endl;
    buf.expand(3000);
    std::cout << buf.size() << " " << buf.reserved() << std::endl;


}
