#ifndef AEvConnection_HPP
#define AEvConnection_HPP

#include "../AEvBase/AEventAbstract.hpp"
#include "SmtpBuffer.hpp"

#include <iostream>

namespace aev {

class AEvSmtpSession : public AEventAbstract
{
public:

    explicit AEvSmtpSession(AEvChildConf && config, asio::ip::tcp::socket && _soc, const smtp::CommandsMap & hm_);
    virtual ~AEvSmtpSession() override;

private:

    asio::ip::tcp::socket _socket;
    SmtpCmdBuffer _read_cmd_buffer;
    SmtpDataBuffer _read_data_buffer;
    smtp::SmtpState _main_smtp_state;
    const smtp::CommandsMap _handlers_map;

protected:
    virtual void _ev_begin() override;
    virtual void _ev_finish() override;
    virtual void _ev_stop() override;
    virtual void _ev_timeout() override;
    virtual void _ev_child_callback(AEvPtrBase child_ptr, AEvExitSignal & _ret) override;

    void _read_command();
    void _transaction();
    void _read_data();
    void _data_acceepted();
    void _respond_handler(std::string data, smtp::ConfirmHendler confirm = nullptr);
    smtp::ReplySendedConfirmHandler _call_mapped_cmd_handler(const std::string & cmd_line, smtp::SmtpErr & err_);
};

} //namespace

#endif // AEvConnection_HPP
