#ifndef GLOBAL_CONF_HPP
#define GLOBAL_CONF_HPP
#include <string>
#include <sys/types.h>
#include <unordered_map>

struct MySQLConf
{
    std::string host;
    std::string user;
    std::string pass;
    int port;
};

struct PluginConfig
{
    // options:
    // global optiong
    uint logging_level = 1;

};

class GlobalConfig
{
public:

    GlobalConfig();

    ~GlobalConfig();

    // chech for config error
    bool have_error();
    bool is_inited();

    // error message
    const std::string &error_text();

    // create global instance of config
    // using example:
    // first init
    // GlobalConfig::instance().read_config("/home/arti/dev/main.conf");
    // using:
    // GlobalConfig conf = GlobalConfig::instance();
    // or PluginConfig conf = GlobalConfig::instance().get_config();
    // or GlobalConfig::instance().get_config();
    void read_config(const std::string &conf_file_path);
    bool set_opt(std::string opt_name, std::string opt_value);
    static GlobalConfig & instance();

    PluginConfig & get_conf();

private:
    bool error_status = false;
    bool inited = true;
    std::string error_message = "Not inited";
    void _trim_comments(std::string & conf_line);
    std::string _trim_first_part(std::string text);
    bool _is_code_begin(const std::string code_line, std::string &part_type);
    bool _read_block(const std::istream & file, std::string & block);

    bool _set_conf_unit(const std::string & Key, const std::string & Value);

    // main option struct.
    // to add new option: 1) add variable in struct PluginConfig. the name of variable must eq optoin name
    // 2) add line CHECK_AND_SET_OPT(config.%option_name%); in _set_conf_unit() func
    // supported 4 types of opt: uint, string, bool and DbConf
    // to add new type - add function overload bool _set_opt(const std::string str_opt, %TYPE% & option)
    PluginConfig config;

    std::string config_path;
};

#endif // GLOBAL_CONF_HPP
