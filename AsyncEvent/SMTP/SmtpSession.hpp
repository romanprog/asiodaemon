#ifndef SMTPSESSION_HPP
#define SMTPSESSION_HPP

#include "../AEvBase/AEventUtilBase.hpp"
#include "SmtpBuffer.hpp"
#include "SmtpTypes.hpp"

#include <string>
#include <vector>
#include <queue>

using SendHendler = std::function<void (std::string answer)>;

class SmtpSession: public aev::AEventUtilBase
{
public:
    SmtpSession(SendHendler cb);

    void transaction(SmtpBuffer &data);
    void begin(std::string && ip);
    bool close_demand();

private:

    SendHendler send_line;
    std::string welcome;
    std::string prim_hostname {"examlpe.my.home"};
    std::vector<smtp::EmailAddr> rcpt_list;
    smtp::EmailAddr mailform;
    smtp::HeloArg helo;
    bool abrt {false};
    std::string dest_ip;
    std::string dest_ip_ptr;

    std::string _helo_cmd(const std::string &args);
    std::string _mail_cmd(std::string cmd_line);
    std::string _rcpt_cmd(std::string cmd_line);



};

#endif // SMTPSESSION_HPP
