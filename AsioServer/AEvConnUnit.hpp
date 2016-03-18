#ifndef AEVCONNUNIT_HPP
#define AEVCONNUNIT_HPP

#include "AEventsAbstract.hpp"
#include "AEvBuffer.hpp"

namespace aev {

class AEvConnUnit : public AEventsAbstract
{
public:

    explicit AEvConnUnit(const AEvChildConf config, asio::ip::tcp::socket _soc);
    virtual ~AEvConnUnit() override
    {
        // std::cout << "AEvConnUnit DESTRUCTOR! " << std::endl;
    }
    std::string test = "sdgsdfg";

private:

    asio::ip::tcp::socket _socket;
    AEvBuffer _read_buf;


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

#endif // AEVCONNUNIT_HPP
