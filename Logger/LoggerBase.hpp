#ifndef LOGGERBASE_H
#define LOGGERBASE_H

#include "../Config/GlobalConf.hpp"

#include <iostream>
#include <string>
#include <sstream>
#include <fstream>


// Log level

enum class LogLevel
{
    none = 0,
    general = 1,
    warn = 2,
    debug = 3
};

/// =========== Base logger ===============
class LoggerBase
{
public:
    LoggerBase();

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

    template<typename ...Args>
    void general_write (const char * pattern, Args && ...args)
    {
        if (GlobalConfig::instance().get_conf().logging_level < static_cast<int>(LogLevel::general))
            return;
        __format_write (pattern,  std::forward<Args>(args)...);

    }


    template<typename ...Args>
    void warn_write (const char * pattern, Args && ...args)
    {
        if (GlobalConfig::instance().get_conf().logging_level < static_cast<int>(LogLevel::warn))
            return;
        __format_write (pattern,  std::forward<Args>(args)...);
    }


    template<typename ...Args>
    void debug_write (const char * pattern, Args && ...args)
    {
        if (GlobalConfig::instance().get_conf().logging_level < static_cast<int>(LogLevel::debug))
            return;
        __format_write (pattern,  std::forward<Args>(args)...);

    }
    virtual ~LoggerBase();

private:

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

class LoggerConsole : public LoggerBase
{
public:

    LoggerConsole();


private:


    void __write_log(const std::string & log_message);
};

/// =============  File logger child ================

class LoggerFile : public LoggerBase
{
public:

    LoggerFile(const std::string && filename);
    ~LoggerFile();


private:

    std::ofstream log_file;
    std::string log_file_name;
    void __write_log(const std::string & log_message);

};


#endif
