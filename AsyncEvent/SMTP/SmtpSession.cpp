#include "SmtpSession.hpp"
#include <iostream>

#include "../DNS/AEvDnsClient.hpp"
#include "../../HUtils/HStrings.hpp"
#include "../../Logger/Logger.hpp"

SmtpSession::SmtpSession(SendHendler cb)
    : send_line(cb),
      welcome("220 Welcome my son, welcome to the machine. SMTP experimental.")
{
    smtp::BaseMod::RegisterCommands(_handlers_map);
}

void SmtpSession::transaction(SmtpCmdBuffer &data)
{
    while (!data.is_empty()) {

        std::string cmd_line(data.get_line());
        log_debug(cmd_line);
        // CLog::glob().debug_write(cmd_line);

        if (_state.waiting_for_data)
        {
            send_line("500 Excess command after DATA in the same transaction.\r\n");
            continue;
        }

        smtp::SmtpErr rerr = _call_mapped_cmd_handler(cmd_line);
        abrt = _state.close_conn;
        if (rerr != smtp::SmtpErr::noerror) {
            send_line(smtp::utils::err_to_str(rerr));
        } else {
            send_line(std::move(_state.curent_reply));
        }

        if (abrt)
            break;
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


smtp::SmtpErr SmtpSession::_call_mapped_cmd_handler(const std::string &cmd_line)
{
    smtp::SmtpErr res;
    try {
        auto functor = _handlers_map.at(smtp::utils::get_cmd_str(cmd_line));
        res = functor(cmd_line, _state);
    }
    catch (std::out_of_range& oor)
    {
        return smtp::SmtpErr::unrecognized;
    }
    return res;
}

