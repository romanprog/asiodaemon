#ifndef AEvConnection_HPP
#define AEvConnection_HPP

#include "../AEvBase/AEventAbstract.hpp"
#include "SmtpBuffer.hpp"
#include "IncSmtpState.hpp"

#include <iostream>

namespace aev {

using ReplySendedConfirmHandler = std::function<void (smtp::IncSmtpState & proto_state)>;
using ConfirmHendler = std::function<void (bool err)>;
using CommandHandler = std::function<ReplySendedConfirmHandler (const std::string & cmd, smtp::IncSmtpState & proto_state, smtp::SmtpErr & err_)>;
using CommandsMap = std::unordered_map<std::string, CommandHandler>;
using SmtpStatePtr = std::shared_ptr<smtp::IncSmtpState>;
using NewMessageHandler = std::function<void (SmtpStatePtr && message_)>;

class AEvSmtpSession : public AEventAbstract
{
public:

    explicit AEvSmtpSession(AEvChildConf && config,
                            asio::ip::tcp::socket && _soc,
                            const CommandsMap & hm_,
                            NewMessageHandler hdl_,
                            ConfigData main_conf_);

    virtual ~AEvSmtpSession() override;

private:

    asio::ip::tcp::socket _socket;
    SmtpCmdBuffer _read_cmd_buffer;
    SmtpDataBuffer _read_data_buffer;
    SmtpStatePtr _main_smtp_state;
    NewMessageHandler _rcv_msg_handler;
    const CommandsMap _handlers_map;
    ConfigData _main_config;

protected:
    virtual void _ev_begin() override;
    virtual void _ev_finish() override;
    virtual void _ev_stop() override;
    virtual void _ev_timeout() override;
    virtual void _ev_child_callback(AEvPtrBase child_ptr, AEvExitSignal & _ret) override;

    void _read_command();
    void _begin_transaction();
    void _read_data();
    void _data_acceepted();
    void _send_respond(std::string data, ConfirmHendler confirm = nullptr);
    ReplySendedConfirmHandler _call_mapped_cmd_handler(const std::string & cmd_line, smtp::SmtpErr & err_);
};

} //namespace

#endif // AEvConnection_HPP
