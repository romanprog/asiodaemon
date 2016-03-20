#ifndef AEvAcceptor_HPP
#define AEvAcceptor_HPP
#include "../Abstract/AEventAbstract.hpp"

namespace aev {

class AEvAcceptor : public AEventAbstract
{
public:

    AEvAcceptor(AEvRootConf & config, const std::string &ip, const unsigned port);
    AEvAcceptor(const AEvChildConf config, const std::string &ip, const unsigned port);

    virtual ~AEvAcceptor() override
    {
       // std::cout << "AEvAcceptor DESTRUCTOR! " << std::endl;
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
    virtual void _ev_child_callback(AEvPtrBase child_ptr, AEvExitSignal & _ret) override;

    void _start_acceept();

};

} //namespace

#endif // AEvAcceptor_HPP
