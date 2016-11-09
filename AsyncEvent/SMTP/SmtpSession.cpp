#include "SmtpSession.hpp"
#include <iostream>

#include "../DNS/AEvDnsClient.hpp"
#include "../../HUtils/HStrings.hpp"
#include "../../Logger/Logger.hpp"

SmtpSession::SmtpSession(SendHendler cb)
    :send_line(cb),
    welcome("220 Welcome my son, welcome to the machine. SMTP experimental.")
{

}

void SmtpSession::transaction(SmtpCmdBuffer &data)
{
    while (!data.is_empty()) {

        std::string cmd_args;
        std::string cmd_line(data.get_line());
        log_debug(cmd_line);
        // CLog::glob().debug_write(cmd_line);

        if (_state.waiting_for_data)
        {
            send_line("500 Excess command after DATA in the same transaction.\r\n");
            continue;
        }

        smtp::SmtpCmd cmd_t = smtp::utils::parse_line(cmd_line);

        switch (cmd_t) {
        case smtp::SmtpCmd::quit:
        {
            abrt = true;
            cmd_args = "221 Bye!\r\n";
            send_line(std::move(cmd_args));
            break;
        }

        case smtp::SmtpCmd::helo:
        {
            smtp::SmtpErr rerr = smtp::utils::parse_helo(cmd_line, cmd_args);
            if (rerr == smtp::SmtpErr::noerror)
                _helo_cmd(cmd_args);
            else
                send_line(smtp::utils::err_to_str(rerr));
            break;
        }

        case smtp::SmtpCmd::mail:
        {
            smtp::EmailAddr _tmp;
            smtp::SmtpErr rerr = smtp::utils::parse_mail_from(cmd_line, _tmp);
            if (rerr == smtp::SmtpErr::noerror)
                _mail_cmd(std::move(_tmp));
            else
                send_line(smtp::utils::err_to_str(rerr));
            break;
        }

        case smtp::SmtpCmd::rcpt:
        {
            smtp::EmailAddr _tmp;
            smtp::SmtpErr rerr = smtp::utils::parse_rcpt_to(cmd_line, _tmp);

            if (rerr == smtp::SmtpErr::noerror)
                _rcpt_cmd(std::move(_tmp));
            else
                send_line(smtp::utils::err_to_str(rerr));
            break;

        }

        case smtp::SmtpCmd::data:
        {
            smtp::SmtpErr rerr = smtp::utils::parse_data(cmd_line);

            if (rerr == smtp::SmtpErr::noerror)
                _data_cmd();
            else
                send_line(smtp::utils::err_to_str(rerr));
            break;

        }

        case smtp::SmtpCmd::unknown:
        {
            send_line(smtp::utils::err_to_str(smtp::SmtpErr::unrecognized));
            break;
        }

        default:
            break;

        }

    }
}

void SmtpSession::accept_data(SmtpDataBuffer &data)
{
    log_debug(data.get_data());
    send_line("250 OK. 1460191710 b128si9046197lfb.\r\n");
    _state.waiting_for_data = false;
}

void SmtpSession::begin(std::string &&ip)
{
    _state.client_ip = std::move(ip);
    _create_child<aev::AEvDnsClient>(0, _state.client_ip, dns::DnsQType::PTR,
                                     [this](int err, dns::DnsRespondPtr && result)
    {
        if (!err)
            for (auto & r : result->alist)
                _state.client_ip_ptr = r.answer;
    });

    send_line(welcome + " " + _state.client_ip_ptr + " " + prim_hostname + " greeting you\r\n");
}

bool SmtpSession::close_demand()
{
    return abrt;
}

bool SmtpSession::read_data_demand()
{
    return _state.waiting_for_data;
}

void SmtpSession::_helo_cmd(const std::string & args)
{
    _state.client_hello.text = args;
    if (dns::utils::is_fqdn(_state.client_hello.text)) {
        _state.client_hello.is_fqdn = true;
        _create_child<aev::AEvDnsClient>(1, _state.client_hello.text, dns::DnsQType::A,
                                         [this](int err, dns::DnsRespondPtr && result)
        {
            if (!err)
                _state.client_hello.ip = dns::utils::get_rand_rec_value(*result);
        });

    }

    std::string reply = "250 " + prim_hostname + " " + (_state.client_ip_ptr.empty() ? _state.client_ip : _state.client_ip_ptr) + "\r\n";
    _state.client_hello.inited = true;
    send_line(reply);
}

void SmtpSession::_mail_cmd(smtp::EmailAddr &&email)
{
    if (!_state.client_hello.inited) {
        send_line(smtp::utils::err_to_str(smtp::SmtpErr::heloneed));
        return;
    }
    _state.mailform = email;

    std::string reply = "250 " + email.text + " Parsed email:" + _state.mailform.text + "\r\n";

    send_line(reply);

}

void SmtpSession::_rcpt_cmd(smtp::EmailAddr &&email)
{

    if (!_state.mailform.inited) {
        send_line(smtp::utils::err_to_str(smtp::SmtpErr::sync));
        return;
    }
    std::string reply;
    auto dupcheck = _state.recipients.list.find(email);
    if (dupcheck == _state.recipients.list.end()) {
        _state.recipients.list.insert(email);
        reply = "250 OK. Parsed email:" + email.text + "\r\n";
    }
    else {
        reply = "253 OK. Parsed email:" + email.text + " Duplicate. Ignored. \r\n";
    }
    send_line(reply);
}

void SmtpSession::_data_cmd()
{
    _state.waiting_for_data = true;
    send_line("354 OK. Redy for accept data. \r\n");
}

