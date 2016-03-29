#include <iostream>
#include <string>
#include <set>
#include <tuple>
#include <map>
#include <memory>

#include "AsyncEvent/SMTP/AEvAcceptor.hpp"
#include "AsyncEvent/AEvBase/AEventTypes.hpp"

auto cb = [](aev::AEvPtrBase tptr, aev::AEvExitSignal sin_num) -> int
{
    std::cout << "aCBACK" << std::endl;
    return 0;
};



int main()
{

    aev::AEvRootConf main_conf(cb);
    std::shared_ptr<aev::AEvAcceptor> conn = std::make_shared<aev::AEvAcceptor>(main_conf, "127.0.0.1", 8888);
    conn->begin();
    conn->run();
}
