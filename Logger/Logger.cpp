#include "Logger.hpp"
#include "../Config/GlobalConf.hpp"

#include <stdarg.h>
#include <fstream>


/// =============== Console child ================

CLog::CLog()
{

}

CLog &CLog::glob()
{
    static CLog global_console_log;
    return global_console_log;
}

void CLog::__write_log(const std::string &log_message)
{
    std::cout << log_message << std::endl;
}


/// =============  File logger child ================

FLog::FLog(const std::string &&filename)
    :log_file_name(filename)
{

    log_file.open( filename, std::ofstream::out | std::ofstream::app );


}

FLog::~FLog()
{

        if (log_file.is_open())
            log_file.close();

}

void FLog::__write_log(const std::string &log_message)
{
    if (!log_file.is_open())
        return;

    log_file << log_message << std::endl;


}
