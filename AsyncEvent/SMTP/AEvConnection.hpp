#ifndef AEvConnection_HPP
#define AEvConnection_HPP

#include "../Abstract/AEventAbstract.hpp"
#include "../../Buffers/SmtpBuffer.hpp"

namespace aev {

class AEvConnection : public AEventAbstract
{
public:

    explicit AEvConnection(const AEvChildConf config, asio::ip::tcp::socket _soc);
    virtual ~AEvConnection() override
    {
        // std::cout << "AEvConnection DESTRUCTOR! " << std::endl;
    }
    std::string test = "sdgsdfg";

private:

    asio::ip::tcp::socket _socket;
    SmtpBuffer _read_buf;


protected:
    virtual void _ev_begin() override;
    virtual void _ev_finish() override;
    virtual void _ev_stop() override;
    virtual void _ev_timeout() override;
    virtual void _ev_child_callback(AEvPtrBase child_ptr, AEvExitSignal & _ret) override;

    void _start_read();
    void _start_send();

};

} //namespace

#endif // AEvConnection_HPP
