#include "GlobalConf.hpp"
#include "../HUtils/HStrings.hpp"


#include <sys/types.h>
#include <fstream>
#include <iostream>
#include <vector>

#define CHECK_AND_SET_OPT(OPT) \
str_tmp = _get_opt_name(#OPT);\
if (Key == str_tmp) { \
    if (!_set_opt(Value, OPT)) { \
        error_message = "Error on option value. Key: " + Key + ", Value: " + Value; \
        error_status = true; \
        return false; \
    } \
    return true; \
} \

bool _set_opt(const std::string &str_opt, bool & option);
bool _set_opt(const std::string &str_opt, uint & option);
bool _set_opt(const std::string &str_opt, std::string & option);
bool _set_opt(std::string str_opt, MySQLConf &option);

bool Config::have_error()
{
    return error_status;
}

bool Config::is_inited()
{
    return inited;
}

const std::string & Config::error_text()
{
    return error_message;
}


bool Config::read_config(const std::string & conf_file_path)
{
    // if not inited
    config = ConfigData();
    inited = false;

    std::ifstream config_file(conf_file_path);

    if (!config_file) {

        error_message = "Can't read file \"" + conf_file_path + "\".";
        error_status = true;
        return false;

    }

    std::string conf_line;

    int line_num = 0;

    while (std::getline(config_file, conf_line))
    {
        ++line_num;
        _trim_comments(conf_line);
        hstrings::trim(conf_line);

        if (conf_line.size() < 1)
            continue;

        // check option
        std::vector<std::string> name_val = hstrings::splitted(conf_line, '=');

        if (name_val.size() == 2)
        {
            std::string key_str = name_val[0];
            std::string key_opt = name_val[1];
            hstrings::trim(key_str);
            hstrings::trim(key_opt);
            if (!_set_conf_unit(key_str, key_opt))
                return false;

            continue;
        }

        std::string code_type;

        // check of runtime code begin
        if (_is_code_begin(conf_line, code_type))
        {
            std::string code;

            int cfpos = config_file.tellg();

            while (std::getline(config_file, conf_line) )
            {
                std::string code_type_tmp;

                // if we find new code part
                if (_is_code_begin(conf_line, code_type_tmp))
                {
                    config_file.seekg(cfpos);
                    break;
                }

                _trim_comments(conf_line);
                code += conf_line;

                cfpos = config_file.tellg();
            }
            _set_conf_unit(code_type, code);
            continue;
        }
        error_message = "Error in config file. Line number: " + std::to_string(line_num) + ", Text: " + conf_line;
        error_status = true;
        return false;
    }

    config_file.close();

    config_path = conf_file_path;
    inited = true;
    return true;
}

bool Config::set_opt(const std::string & opt_name, const std::string & opt_value)
{
    return _set_conf_unit(opt_name, opt_value);
}


Config &Config::glob()
{
    static Config global_instance_of_config;
    return  global_instance_of_config;
}


ConfigData &Config::get_conf()
{
    return config;
}


void Config::_trim_comments(std::string & conf_line)
{
    size_t pos = conf_line.find_first_of("#");

    if (pos == std::string::npos)
        return;

    conf_line.resize(pos);
}

std::string Config::_get_opt_name(std::string conf_struct_unit)
{
    hstrings::cut_part(conf_struct_unit, '.', 0);
    hstrings::trim(conf_struct_unit);
    return conf_struct_unit;
}

bool Config::_is_code_begin(const std::string & code_line, std::string & part_name)
{
    std::string command;
    for (int i = 0; i < code_line.size(); ++i)
    {
        if ((isalnum(code_line[i]) || code_line[i] == '_' || code_line[i] == ':'))
            command += code_line[i];
    }

    if (command[command.length() - 1] == ':')
    {
        part_name = command;
        // trim ':'
        part_name.resize(part_name.size() - 1);
        return true;
    }

    return false;
}

bool Config::_set_conf_unit(const std::string &Key, const std::string &Value)
{
    // check and set all options
    std::string str_tmp;

    CHECK_AND_SET_OPT(config.logging_level);
    CHECK_AND_SET_OPT(config.redis_host);
    CHECK_AND_SET_OPT(config.redis_port);
    CHECK_AND_SET_OPT(config.primary_hostname);
    CHECK_AND_SET_OPT(config.smtp_welcome);

    error_message = "Unknown option: " + Key;
    error_status = true;
    return false;
}

bool _set_opt(const std::string &str_opt, bool & option)
{
    if (str_opt == "true") {
        option = true;
        return true;
    }

    if (str_opt == "false"){
        option = false;
        return true;
    }

    return false;
}

bool _set_opt(const std::string & str_opt, uint &option)
{
    if (hstrings::is_digit_only(str_opt)) {
        option = std::stoi(str_opt);
        return true;
    }
    return false;
}

bool _set_opt(const std::string & str_opt, std::string & option)
{
    if (str_opt.length() > 0) {
        option = str_opt;
        return true;
    }

    return false;
}

bool _set_opt(std::string str_opt, MySQLConf & option)
{
    std::vector<std::string> db_conf_arr;
    int todo = 1; // Verify mysql string (for special symbols).

//    Spec_Clear(conf_line_cleared, " ");
    hstrings::split(str_opt, db_conf_arr, '/', true);

    if ( db_conf_arr.size() == 3 ) {
        option.host = db_conf_arr[0];
        option.user = db_conf_arr[1];
        option.pass = db_conf_arr[2];
        option.port = 0; // default
        return true;
    }
    return false;
}
