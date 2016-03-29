#include "SmtpSession.hpp"

SmtpSession::SmtpSession(SendHendler cb)
    :send_line(cb),
    welcome("220 Welcome my son, welcome to the machine. SMTP experimental. My Email: <roman.progonnyj@gmail.com>\r\n")
{

}

void SmtpSession::processing(std::string &&datapart)
{
    using namespace smtp;
    std::string smtp_answer;
    SmtpCmd cmd_t = SmtpUtils::cmd_define(datapart);

    switch (cmd_t) {
    case SmtpCmd::quit:
        abrt = true;
        smtp_answer = "Bye!\r\n";
        send_line(std::move(smtp_answer));
        return;
    case SmtpCmd::unknown:
        smtp_answer = "502 Unrecognized command\r\n";
        send_line(std::move(smtp_answer));
        return;
    default:
        break;
    }



//    parsed_cmd = cmd_line;
//    if (cmd_line == "quit") {
//        waiting_for_command = false;
//        abort = true;
//        have_answer = false;
//        _when_have_ansver_cb();
//        return;
//    }

//    _create_child<aev::AEvDnsClient>(0, cmd_line, dns::DnsQType::A,
//                                     [this](int err, dns::DnsRespond result)
//    {
//        if (err) {
//            smtp_answer = "Can't resolve.\r\n";
//        } else {
//            for (auto & r : result.get_answers_list())
//                smtp_answer += r.answer + "\r\n";
//        }
//        have_answer = true;
//        waiting_for_command = false;
//        _when_have_ansver_cb();

//    }
    //    );
}

void SmtpSession::begin()
{
    send_line(welcome);
}

bool SmtpSession::close_demand()
{
    return abrt;
}

