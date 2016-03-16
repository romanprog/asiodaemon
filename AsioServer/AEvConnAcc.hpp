#ifndef AEVCONNACC_HPP
#define AEVCONNACC_HPP
#include "AEventsAbstract.hpp"
#include "AEvBuffer.hpp"

namespace aev {

class AEvConnAcc : public AEventsAbstract
{
public:

    AEvConnAcc(AEvRootConf & config, const std::string &ip, const unsigned port);
    AEvConnAcc(const AEvChildConf config, const std::string &ip, const unsigned port);

    virtual ~AEvConnAcc() override
    {
       // std::cout << "AEvConnAcc DESTRUCTOR! " << std::endl;
    }

private:

    asio::ip::tcp::acceptor _acceptor;
    asio::ip::tcp::socket _socket;
    std::string _conn_ip;
    unsigned _conn_port;

    virtual void _ev_begin() override;
    virtual void _ev_finish() override;
    virtual void _ev_stop() override;
    virtual void _ev_timeout() override;
    virtual void _ev_child_callback(AEvExitSignal _ret) override;

    void _start_acceept();

};

} //namespace

#endif // AEVCONNACC_HPP
