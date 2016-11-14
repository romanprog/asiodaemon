#include "AEvConnection.hpp"
#include "../DNS/AEvDnsClient.hpp"
#include <functional>

#include "iostream"

namespace aev {

AEvSmtpSession::AEvSmtpSession(AEvChildConf &&config, asio::ip::tcp::socket &&_soc, const CommandsMap & hm_, NewMessageHandler hdl_)
    :AEventAbstract::AEventAbstract(std::move(config)),
     _socket(std::move(_soc)),
     _main_smtp_state(std::make_shared<smtp::IncSmtpState>()),
     _rcv_msg_handler(hdl_),
     _handlers_map(hm_)
{
     log_debug("AEvConnection CONSTRUCTOR! ");
}

AEvSmtpSession::~AEvSmtpSession()
{
    log_debug("AEvConnection DESTRUCT" );
}

void AEvSmtpSession::_ev_begin()
{
    log_debug("AEvConnection START");
    _send_respond("220 " + Config::glob().get_conf().smtp_welcome + "\r\n");
    _read_command();
}

void AEvSmtpSession::_ev_finish()
{
    log_debug("AEvConnection FINISH");
}

void AEvSmtpSession::_ev_stop()
{
    log_debug("AEvConnection STOP");
    _ev_exit_signal = AEvExitSignal::close_connection;
    _socket.cancel();
    _socket.close();
}

void AEvSmtpSession::_ev_timeout()
{
    log_debug("AEvConnection TIMEOUT");
}

void AEvSmtpSession::_ev_child_callback(AEvPtrBase child_ptr, AEvExitSignal &_ret)
{

}

void AEvSmtpSession::_read_command()
{
    _read_cmd_buffer.release(1024);

    _socket.async_read_some(asio::buffer(_read_cmd_buffer.data_top(), _read_cmd_buffer.size_avail()),
                            _ev_loop->wrap([this](std::error_code ec, std::size_t bytes_transferred){

                                if (ec) {
                                    stop();
                                    return;
                                }
                                _read_cmd_buffer.accept(bytes_transferred);
                                _transaction();
                                _read_cmd_buffer.mem_reduce();

                                if (!_main_smtp_state->waiting_for_data) {
                                    _read_command();
                                }
                                else {
                                    _main_smtp_state->message_data->clear();
                                    _read_data();
                                }
                            }));
}

void AEvSmtpSession::_transaction()
{
    if (_read_cmd_buffer.is_empty()) {
        return;
    }

    std::string cmd_line(_read_cmd_buffer.get_line());
    log_debug(cmd_line);

    if (_main_smtp_state->waiting_for_data)
    {
        _send_respond("500 Excess command after DATA in the same transaction.\r\n");
        _transaction();
        return;
    }

    smtp::SmtpErr rerr;
    auto confirm_handler = _call_mapped_cmd_handler(cmd_line, rerr);

    auto confirm_handler_wrapper = [confirm_handler, this](bool err)
    {
        if (!err && confirm_handler)
            confirm_handler(*_main_smtp_state);
    };

    if (rerr == smtp::SmtpErr::noerror) {
        _send_respond(std::move(_main_smtp_state->curent_reply), confirm_handler_wrapper);
    } else {
        _send_respond(smtp::utils::err_to_str(rerr));
    }

    if (_main_smtp_state->close_conn)
        return;

    _transaction();
}

void AEvSmtpSession::_read_data()
{
    _main_smtp_state->message_data->release(70000);

    _socket.async_read_some(asio::buffer(_main_smtp_state->message_data->data_top(), _main_smtp_state->message_data->size_avail()),
                            _ev_loop->wrap([this](std::error_code ec, std::size_t bytes_transferred)
    {

                                if (ec) {
                                    stop();
                                    return;
                                }
                                _main_smtp_state->message_data->accept(bytes_transferred);

                                if (!_main_smtp_state->message_data->is_redy()) {
                                    _read_data();
                                } else {
                                    _data_acceepted();
                                }
                            } ));
}

void AEvSmtpSession::_data_acceepted()
{
    _send_respond("250 OK. 1460191710 b128si9046197lfb.\r\n");
    SmtpStatePtr state_for_handling = _main_smtp_state;
    _main_smtp_state.reset();
    _main_smtp_state = std::make_shared<smtp::IncSmtpState>();
    _rcv_msg_handler(std::move(state_for_handling));
    _main_smtp_state->client_hello = state_for_handling->client_hello;
    _main_smtp_state->reset_transaction();
    _read_command();
}

void AEvSmtpSession::_send_respond(std::string data, ConfirmHendler confirm)
{

    _socket.async_send(asio::buffer(data),
                       _ev_loop->wrap([this, confirm](std::error_code ec, std::size_t bytes_transferred)
    {
        if (ec) {
            stop();
            return;
        }
        if (_main_smtp_state->close_conn) {
            stop();
            return;
        };

        if (confirm) {
            confirm(false);
        }
        reset_and_start_timer();
    }));
}

ReplySendedConfirmHandler AEvSmtpSession::_call_mapped_cmd_handler(const std::string &cmd_line, smtp::SmtpErr &err_)
{
    ReplySendedConfirmHandler res_handler;
    try {
        auto functor = _handlers_map.at(smtp::utils::get_cmd_str(cmd_line));
        res_handler = functor(cmd_line, *_main_smtp_state, err_);
        return res_handler;
    }
    catch (std::out_of_range& oor)
    {
        err_ = smtp::SmtpErr::unrecognized;
        return nullptr;
    }
    return nullptr;
}

} //namespace


