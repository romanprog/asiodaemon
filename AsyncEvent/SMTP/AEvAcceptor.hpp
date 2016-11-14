#ifndef AEvAcceptor_HPP
#define AEvAcceptor_HPP
#include "../AEvBase/AEventAbstract.hpp"
#include "AEvConnection.hpp"
#include "SmtpTypes.hpp"

namespace aev {

class AEvAcceptor : public AEventAbstract
{
public:

    AEvAcceptor(AEvRootConf & config, const std::string &ip, const unsigned port);
    AEvAcceptor(AEvChildConf &&config, const std::string &ip, const unsigned port);

    virtual ~AEvAcceptor() override
    {
       log_debug("AEvAcceptor DESTRUCTOR! ");
    }

private:

    asio::ip::tcp::acceptor _acceptor;
    asio::ip::tcp::socket _socket;
    std::string _conn_ip;
    unsigned _conn_port;
    // SMTP protocol commands map. "cmd" -> cmd_handler
    CommandsMap _handlers_map;
    size_t _msg_counter {0};

    virtual void _ev_begin() override;
    virtual void _ev_finish() override;
    virtual void _ev_stop() override;
    virtual void _ev_timeout() override;
    virtual void _ev_child_callback(AEvPtrBase child_ptr, AEvExitSignal & _ret) override;

    void _start_acceept();
    void _new_message_handler(SmtpStatePtr && message_);

};

} //namespace

#endif // AEvAcceptor_HPP
