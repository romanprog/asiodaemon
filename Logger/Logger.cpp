#include "Logger.hpp"
#include "../Config/GlobalConf.hpp"

#include <stdarg.h>
#include <fstream>

/// =============== Console child ================

CLog::CLog(unsigned lev)
    : Log::Log(lev)
{

}

CLog &CLog::glob()

{
    static CLog global_console_log(Config::glob().get_conf().logging_level);
    return global_console_log;
}

void CLog::__write_log(const std::string &log_message)
{
    std::cout << log_message << std::endl;
}

/// =============  File logger child ================

FLog::FLog(const std::string &&filename, unsigned lev)
    :Log::Log(lev), log_file_name(filename)
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

Log &Log::glob()
{
    static Log global_log_utils{Config::glob().get_conf().logging_level};
    return global_log_utils;
}

void Log::change_llevel(unsigned ll_)
{
    if (ll_ > 3)
        throw std::logic_error("Logging level value is out of range (0-3)");

    _llevel = static_cast<LogLevel>(ll_);
}
