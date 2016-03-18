#include "AEvConnUnit.hpp"

#include "iostream"

namespace aev {

AEvConnUnit::AEvConnUnit(const AEvChildConf config, asio::ip::tcp::socket _soc)
    :AEventsAbstract::AEventsAbstract(config),
     _socket(std::move(_soc))
{
    // std::cout << "AEvConnUnit CONSTRUCTOR! " << std::endl;
}

void AEvConnUnit::_ev_begin()
{
    _start_send();
}

void AEvConnUnit::_ev_finish()
{

}

void AEvConnUnit::_ev_stop()
{
    _ev_exit_signal = AEvExitSignal::close_connection;
    _socket.cancel();
    _socket.close();
}

void AEvConnUnit::_ev_timeout()
{

}

void AEvConnUnit::_ev_child_callback(AEvPtrBase child_ptr, AEvExitSignal &_ret)
{

}



void AEvConnUnit::_start_read()
{
    _read_buf.release(80);

    _socket.async_read_some(asio::buffer(_read_buf.data_top(), _read_buf.size_avail()),
                            _ev_loop->wrap([this](std::error_code ec, std::size_t bytes_transferred){

                                if (ec) {
                                    return;
                                }
                                if (!_read_buf.accept(bytes_transferred)) {
                                    // imposible but...
                                    stop();
                                    return;
                                }

                                if (_read_buf.smtp_error || _read_buf.abort) {
                                    stop();
                                    return;
                                }

                                if (_read_buf.have_answer)
                                {
                                    test =   _read_buf.get_answer();
                                    _start_send();
                                    return;
                                }

                                reset_and_start_timer();

                                _start_read();
                            }));

}

void AEvConnUnit::_start_send()
{

    if (!_read_buf.have_answer || _read_buf.abort)
    {
        stop();
        return;
    }

    _socket.async_send(asio::buffer(_read_buf.answer()),
                       _ev_loop->wrap([this](std::error_code ec, std::size_t bytes_transferred){

                           if (ec) {
                               return;
                           }

                           if (_read_buf.waiting_for_command) {
                               _start_read();
                               return;

                           }
                           reset_and_start_timer();
                           _start_read();
                       }));
}


} //namespace


