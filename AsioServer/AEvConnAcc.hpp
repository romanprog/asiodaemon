#ifndef AEVCONNACC_HPP
#define AEVCONNACC_HPP
#include "AEventsAbstract.hpp"

namespace aev {

class AEvConnAcc : public AEventsAbstract
{
public:

    AEvConnAcc(AEvRootConf & config, const std::string &ip, const unsigned port);
    AEvConnAcc(const AEvChildConf config, const std::string &ip, const unsigned port);

private:

    asio::ip::tcp::acceptor _acceptor;
    asio::ip::tcp::socket _socket;
    std::string _conn_ip;
    unsigned _conn_port;

    virtual void _ev_begin();
    virtual void _ev_finish();
    virtual void _ev_stop();
    virtual void _ev_timeout();
    virtual void _ev_child_callback(int _ret);

    void _start_acceept();

};

} //namespace

#endif // AEVCONNACC_HPP
