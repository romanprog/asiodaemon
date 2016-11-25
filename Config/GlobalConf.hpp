#ifndef GLOBAL_CONF_HPP
#define GLOBAL_CONF_HPP
#include <string>
#include <sys/types.h>
#include <unordered_map>

/// //////////////////////////////////////////////////////////////////
/// Create global instance of config.
/// Using example.
/// 1) first init:
/// GlobalConfig::instance().read_config("/home/arti/dev/main.conf");
/// 2) using:
/// GlobalConfig conf = GlobalConfig::instance();
/// or PluginConfig conf = GlobalConfig::instance().get_config();
/// or GlobalConfig::instance().get_config(); .
///
/// For new option:
/// 1) add variable in struct PluginConfig. the name of variable must eq optoin name
/// 2) add line CHECK_AND_SET_OPT(config.%option_name%); in _set_conf_unit() func
/// supported 4 types of opt: uint, string, bool and DbConf.
/// For new opt type - add function overload
/// bool _set_opt(const std::string str_opt, %TYPE% & option).
/// ///////////////////////////////////////////////////////////////////

struct MySQLConf
{
    std::string host;
    std::string user;
    std::string pass;
    int port;
};

struct ConfigData
{
    // options:
    uint logging_level {1};
    std::string redis_host;
    uint redis_port;
    std::string primary_hostname;
    std::string smtp_welcome;

};

class Config
{
public:

    // Error message.
    const std::string &error_text();
    // Read config from file @conf_file_path.
    // If an error occurs - return false.
    bool read_config(const std::string &conf_file_path);
    // For the manual options setting.
    bool set_opt(const std::string &opt_name, const std::string &opt_value);
    // Chech for config errors.
    bool have_error();
    bool is_inited();
    // Create a global config state for use in any part of the application.
    static Config & glob();
    // Return config struct reference.
    ConfigData & get_conf();

private:
    // Deletes comments from config line.
    void _trim_comments(std::string & conf_line);
    // Return option name from full name of PluginConfig member.
    std::string _get_opt_name(std::string conf_struct_unit);
    // If @code_line is begin of runing code section - return true and write section
    // name to @part_name.
    bool _is_code_begin(const std::string &code_line, std::string &part_name);
    // Set PluginConfig member unit from parsed to {key,value} config line.
    bool _set_conf_unit(const std::string & Key, const std::string & Value);

    // Have error.
    bool error_status = false;
    // File readed, config units inited. No errors.
    bool inited = true;
    std::string error_message = "Not inited";
    // Main config struct.
    ConfigData config;
    // Path to config file.
    std::string config_path;
};

#endif // GLOBAL_CONF_HPP
