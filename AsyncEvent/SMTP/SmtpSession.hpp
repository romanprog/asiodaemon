#ifndef SMTPSESSION_HPP
#define SMTPSESSION_HPP

#include "../AEvBase/AEventUtilBase.hpp"
#include "SmtpBuffer.hpp"
#include "SmtpTypes.hpp"

#include <string>
#include <vector>
#include <queue>

using ConfirmHendler = std::function<void (bool err)>;
using SendHendler = std::function<void (std::string answer)>;
// using SendWithConfirmHendler = std::function<void (std::string answer, ConfirmHendler confirm)>;

class SmtpSession: public aev::AEventUtilBase
{
public:
    SmtpSession(SendHendler cb);

    void transaction(SmtpCmdBuffer &data);
    void accept_data(SmtpDataBuffer &data);
    void begin(std::string && ip);
    bool close_demand();
    bool read_data_demand();

private:

    SendHendler send_line;
    // SendWithConfirmHendler send_and_confirm_line;
    std::string welcome;
    std::string prim_hostname {"examlpe.my.home"};
    smtp::SmtpState _state;
    bool abrt {false};


    void _helo_cmd(const std::string &args);
    void _mail_cmd(smtp::EmailAddr &&email);
    void _rcpt_cmd(smtp::EmailAddr &&email);
    void _data_cmd();



};

#endif // SMTPSESSION_HPP
