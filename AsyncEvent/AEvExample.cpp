#include "AEvExample.hpp"

#include "iostream"

namespace aev {

AEvTemplate::AEvTemplate(const AEvChildConf config, asio::ip::tcp::socket _soc)
    :AEventsAbstract::AEventsAbstract(config),
     _socket(std::move(_soc))
{
    // std::cout << "AEvConnection CONSTRUCTOR! " << std::endl;
}

void AEvTemplate::_ev_begin()
{
    _start_read();
}

void AEvTemplate::_ev_finish()
{

}

void AEvTemplate::_ev_stop()
{
    _ev_exit_signal = AEvExitSignal::close_connection;
    _socket.cancel();
    _socket.close();
}

void AEvTemplate::_ev_timeout()
{

}


void AEvTemplate::_start_read()
{
    _read_buf.release(40);
    std::cout << "accept, free data: "<< _read_buf.size_avail() << std::endl;

    _socket.async_read_some(asio::buffer(_read_buf.data_top(), _read_buf.size_avail()),
                           _ev_loop->wrap([this](std::error_code ec, std::size_t bytes_transferred){

        if (ec) {
            return;
        }
        // std::cout << "read" << std::endl;
        if (_read_buf.accept(bytes_transferred)) {

            for (auto & str_ : aev::get_buff_dala_list(_read_buf))
                 std::cout << str_ << ";" << std::endl;

            // std::cout << _read_buf.get() << ", Overdata: " << _read_buf.redundant_data_size() << std::endl;

            // _read_buf.reset();
        }
        reset_and_start_timer();

        _start_read();
    }));

}

void aev::AEvTemplate::_ev_child_callback(AEvPtrBase child_ptr, AEvExitSignal &_ret)
{

}


} //namespace
