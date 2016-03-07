#include <iostream>
#include <string>
#include <set>
#include <tuple>
#include <map>
#include <memory>

#include "AsioServer/AEvConnAcc.hpp"
#include "AsioServer/AEventsTypes.hpp"

auto cb = [](aev::AEvPtrBase tptr, int sin_num) -> int
{
    std::cout << "aCBACK" << std::endl;
    return 0;
};



int main()
{

    aev::AEvRootConf main_conf(cb);
    main_conf.timeout = 3;
    aev::AEvConnAcc conn(main_conf, "127.0.0.1", 8888);
    conn.begin();
    main_conf.evloop->get_io_service().run();

}
