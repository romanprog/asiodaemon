#ifndef AEvConnection_HPP
#define AEvConnection_HPP

#include "../AEvBase/AEventAbstract.hpp"
#include "SmtpBuffer.hpp"
#include "SmtpSession.hpp"

#include <iostream>

namespace aev {

class AEvConnection : public AEventAbstract
{
public:

    explicit AEvConnection(const AEvChildConf config, asio::ip::tcp::socket _soc);
    virtual ~AEvConnection() override {}

private:

    asio::ip::tcp::socket _socket;
    SmtpCmdBuffer read_cmd_buffer;
    SmtpDataBuffer read_data_buffer;
    SmtpSession session;

protected:
    virtual void _ev_begin() override;
    virtual void _ev_finish() override;
    virtual void _ev_stop() override;
    virtual void _ev_timeout() override;
    virtual void _ev_child_callback(AEvPtrBase child_ptr, AEvExitSignal & _ret) override;

    void _read_command();
    void _read_data();
    void _respond_handler(std::string data, ConfirmHendler confirm = nullptr);

};

} //namespace

#endif // AEvConnection_HPP
