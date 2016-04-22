#include <iostream>
#include <string>

#include "HUtils/HStrings.hpp"

int main () {


    // To learn and using:
    std::accumulate();

    RedisRBuffer rbuff;

    std::string big_data;
    for (int i = 0; i < 10000; ++i)
        big_data += "-Error message bla bla bla bla bla bla \r\n";

    size_t beg_pos{0}, sub_str_size{0};
    std::srand(std::time(0));
    int counter{0};

    redis::RespData resp;

    while (true)
    {
        rbuff.release(big_data.size()/100 + 11);

        sub_str_size = (rand() % big_data.size()/1000) + 10;
        if ((beg_pos + sub_str_size) > big_data.size())
            sub_str_size = big_data.size() - beg_pos;
        memcpy(rbuff.data_top(), big_data.data() + beg_pos, sub_str_size);
        rbuff.accept(sub_str_size);
        while (rbuff.parse_one(resp))
        {
            log_main("% %", sub_str_size, resp.sres);
        }
        ++counter;
        beg_pos += sub_str_size;
        if (beg_pos >= big_data.size())
            break;

    }

}


