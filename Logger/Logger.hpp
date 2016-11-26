#ifndef LOGGERBASE_H
#define LOGGERBASE_H

#include "../Config/GlobalConf.hpp"

#include <iostream>
#include <string>
#include <sstream>
#include <fstream>

#define log_main(fmt, ...) CLog::glob().write(fmt, ##__VA_ARGS__)
#define log_warn(fmt, ...) CLog::glob().warn_write(fmt, ##__VA_ARGS__)
#define log_debug(fmt, ...) CLog::glob().debug_write("Debug print: File: % (%) on line %. Message: % ", __FILE__, __FUNCTION__, __LINE__, CLog::glob().log_format_to_str(fmt, ##__VA_ARGS__))
#define log_debug_aev(fmt, ...) CLog::glob().debug_write("Debug print: File: % (%) on line %. Message: %, ID: % ", __FILE__, __FUNCTION__, __LINE__, CLog::glob().log_format_to_str(fmt, ##__VA_ARGS__), debug_get_id())
// AEventAbstract::debug_get_id()
// Log level

enum class LogLevel
{
    none = 0,
    general = 1,
    warn = 2,
    debug = 3
};

/// ////////////////////////// Base logger //////////////////////////////////
/// String format: (patern, arg1, arg2..., argN).
/// Patern - string with replaceable symbol '%',  which will be replaced by
/// the corresponding argument. Arg must have standart ostream owerload.
/// Example (console logger):
/// int i {100}, i1 {1000};
/// std::string str("String arg.");
/// CLog::glob().write("i = %; i1 = %; str = %;", i, i1, str);;
/// /////////////////////////////////////////////////////////////////////////
class Log
{
public:
    Log(const unsigned logging_level)
        :_llevel(static_cast<LogLevel>(logging_level))
    {}
    virtual ~Log() {}

    // Standart log write. (Typically, it's errors, and standart log information).
    template<typename ...Args>
    void write (const std::string & pattern, Args && ...args)
    {
        if (_llevel < LogLevel::general)
            return;
        __format_write (pattern.c_str(),  std::forward<Args>(args)...);
    }

    // Warn write = standart write + warning information.
    template<typename ...Args>
    void warn_write (const std::string & pattern, Args && ...args)
    {
        if (_llevel < LogLevel::warn)
            return;
        __format_write (pattern.c_str(),  std::forward<Args>(args)...);
    }

    // Full logging.
    template<typename ...Args>
    void debug_write (const std::string & pattern, Args && ...args)
    {
        if (_llevel < LogLevel::debug)
            return;
        __format_write (pattern.c_str(),  std::forward<Args>(args)...);
    }

    template<typename ...Args>
    const std::string log_format_to_str(const std::string & pattern, Args && ...args)
    {
        std::string result;
        try
        {
            _get_str(result, pattern.c_str(),  std::forward<Args>(args)...);
        }
        catch (...)
        {
            // generate error
            return result;
        }

        return result;
    }

    inline const std::string &log_format_to_str(const std::string & pattern)
    {
        return pattern;
    }

    static Log & glob();

    void change_llevel(unsigned ll_);


private:

    LogLevel _llevel;

    template<typename ...Args>
    void inline __format_write (const char * pattern, Args && ...args)
    {
        std::string log_message;
        try
        {
            _get_str(log_message, pattern,  std::forward<Args>(args)...);
        }
        catch (...)
        {
            // generate error
            return;
        }

        __write_log(log_message);

    }

    void _get_str (std::string & result, const char * pattern)
    {
        while (*pattern) {
            if (*pattern == '%') {
                if (*(pattern + 1) == '%') {
                    ++pattern;
                }
                else {
                    throw std::runtime_error("invalid format string: missing arguments");
                }
            }
            result += *pattern++;
        }
    }

    template<typename T,typename ...Args>
    void _get_str (std::string & result, const char * pattern, T value, Args && ...args)
    {
        while (*pattern) {
            if (*pattern == '%') {
                if (*(pattern + 1) == '%') {
                    ++pattern;
                }
                else {
                    std::stringstream stream;
                    stream << value;
                    result += stream.str();
                    _get_str(result, pattern + 1, args...);
                    return;
                }
            }
            result += *pattern++;
        }
        throw std::logic_error("extra arguments provided to printf");
    }

    virtual void __write_log(const std::string & log_message) {}
};

/// ============  Console child =============

class CLog : public Log
{
public:
    CLog(unsigned lev);
    static CLog & glob();

private:
    void __write_log(const std::string & log_message);
};

/// =============  File logger child ================

class FLog : public Log
{
public:
    FLog(const std::string &&filename, unsigned lev);
    ~FLog();
private:
    std::ofstream log_file;
    std::string log_file_name;
    void __write_log(const std::string & log_message);
};


#endif
