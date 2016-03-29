#ifndef SMTPSESSION_HPP
#define SMTPSESSION_HPP

#include "../AEvBase/AEventUtilBase.hpp"
#include "SmtpTypes.hpp"

#include <string>
#include <vector>

using SendHendler = std::function<void (std::string answer)>;

class SmtpSession: public aev::AEventUtilBase
{
public:
    SmtpSession(SendHendler cb);

    void processing(std::string && datapart);
    void begin();
    bool close_demand();

private:

    SendHendler send_line;
    std::string welcome;
    std::vector<smtp::EmailAddr> rcpt_list;
    smtp::EmailAddr mailform;
    smtp::HeloArg helo;
    bool abrt {false};


};

#endif // SMTPSESSION_HPP
