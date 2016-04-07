#include "SmtpSession.hpp"

#include "../DNS/AEvDnsClient.hpp"
#include "../../HUtils/HStrings.hpp"

SmtpSession::SmtpSession(SendHendler cb)
    :send_line(cb),
    welcome("220 Welcome my son, welcome to the machine. SMTP experimental. My Email: <roman.progonnyj@gmail.com>")
{

}

void SmtpSession::transaction(SmtpBuffer &data)
{
    while (!data.is_empty()) {

        std::string cmd_args;
        std::string cmd_line(data.get_line());
        smtp::SmtpCmd cmd_t = smtp::utils::parse_line(cmd_line);

        switch (cmd_t) {
        case smtp::SmtpCmd::quit:
            abrt = true;
            cmd_args = "221 Bye!\r\n";
            send_line(std::move(cmd_args));
            break;
        case smtp::SmtpCmd::helo:
        {
            smtp::SmtpErr rerr = smtp::utils::parse_helo(cmd_line, cmd_args);
            if (rerr == smtp::SmtpErr::noerror)
                send_line(_helo_cmd(cmd_args));
            else
                send_line(smtp::utils::err_to_str(rerr));
            break;
        }
        case smtp::SmtpCmd::mail:
        {
            if (!helo.inited) {
                send_line(smtp::utils::err_to_str(smtp::SmtpErr::heloneed));
                break;
            }
            smtp::SmtpErr rerr = smtp::utils::parse_mail_from(cmd_line, mailform);
            if (rerr == smtp::SmtpErr::noerror)
                send_line(_mail_cmd(cmd_line));
            else
                send_line(smtp::utils::err_to_str(rerr));
            break;
        }
        case smtp::SmtpCmd::unknown:
            send_line(smtp::utils::err_to_str(smtp::SmtpErr::unrecognized));
            break;
        default:
            break;
        }

    }


}

void SmtpSession::begin(std::string &&ip)
{
    dest_ip = ip;
    _create_child<aev::AEvDnsClient>(1, dest_ip, dns::DnsQType::PTR,
                                     [this](int err, dns::DnsRespond result)
    {
        if (!err)
            for (auto & r : result.alist)
                dest_ip_ptr = r.answer;


    });

    send_line(welcome + " " + dest_ip_ptr + " " + prim_hostname + " greeting you\r\n");
}

bool SmtpSession::close_demand()
{
    return abrt;
}

std::string SmtpSession::_helo_cmd(const std::string & args)
{
    helo.text = args;
    if (dns::utils::is_fqdn(helo.text)) {
        helo.is_fqdn = true;
        _create_child<aev::AEvDnsClient>(1, helo.text, dns::DnsQType::A,
                                         [this](int err, dns::DnsRespond result)
        {
            std::string test;
            if (!err) {
//                for (auto & r : result.alist)
//                    helo.ip = r.answer;


                test += dns::utils::get_rand_rec_value(result) + "\r\n";

            } else {
                if (err == static_cast<int>(dns::DnsError::timeout_err))
                    test = "Resolve timeout\r\n";
                else
                    test = "Some error\r\n";

            }
            send_line(test);
        });

    }

    helo.inited = true;
    return "250 " + prim_hostname + " " + (dest_ip_ptr.empty() ? dest_ip : dest_ip_ptr) + "\r\n";
}

std::string SmtpSession::_mail_cmd(std::string cmd_line)
{
    return "250 " + cmd_line + " Parsed email:" + mailform.text + "\r\n";
}

std::string SmtpSession::_rcpt_cmd(std::string cmd_line)
{

}

