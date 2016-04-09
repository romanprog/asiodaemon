#include "AEvExample.hpp"

#include "iostream"

namespace aev {

AEvExample::AEvExample(const AEvChildConf config, asio::ip::tcp::socket _soc)
    :AEventsAbstract::AEventsAbstract(config),
     _socket(std::move(_soc))
{
    // log_debug("AEvConnection CONSTRUCTOR! ");
}

void AEvExample::_ev_begin()
{
    _start_read();
}

void AEvExample::_ev_finish()
{

}

void AEvExample::_ev_stop()
{
    _ev_exit_signal = AEvExitSignal::close_connection;
    _socket.cancel();
    _socket.close();
}

void AEvExample::_ev_timeout()
{

}


void AEvExample::_start_read()
{
    _read_buf.release(40);
    log_debug("accept, free data: "<< _read_buf.size_avail());

    _socket.async_read_some(asio::buffer(_read_buf.data_top(), _read_buf.size_avail()),
                           _ev_loop->wrap([this](std::error_code ec, std::size_t bytes_transferred){

        if (ec) {
            return;
        }
        if (_read_buf.accept(bytes_transferred)) {

            for (auto & str_ : aev::get_buff_dala_list(_read_buf))
                 log_debug(str_ << ";");

        }
        reset_and_start_timer();

        _start_read();
    }));

}

void aev::AEvExample::_ev_child_callback(AEvPtrBase child_ptr, AEvExitSignal &_ret)
{

}


} //namespace
