#include "LoggerBase.hpp"
#include "../Config/GlobalConf.hpp"

#include <stdarg.h>
#include <fstream>

///=================== Log base ===================
LoggerBase::LoggerBase()
{

}



LoggerBase::~LoggerBase()
{

}



/// =============== Console child ================

LoggerConsole::LoggerConsole()
{

}

void LoggerConsole::__write_log(const std::string &log_message)
{
    std::cout << log_message << std::endl;
}


/// =============  File logger child ================

LoggerFile::LoggerFile(const std::string &&filename)
    :log_file_name(filename)
{

    log_file.open( filename, std::ofstream::out | std::ofstream::app );


}

LoggerFile::~LoggerFile()
{

        if (log_file.is_open())
            log_file.close();

}

void LoggerFile::__write_log(const std::string &log_message)
{
    if (!log_file.is_open())
        return;

    log_file << log_message << std::endl;


}
