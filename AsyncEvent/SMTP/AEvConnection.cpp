#include "AEvConnection.hpp"
#include "../DNS/AEvDnsClient.hpp"
#include <functional>


#include "iostream"

namespace aev {

AEvConnection::AEvConnection(const AEvChildConf config, asio::ip::tcp::socket _soc)
    :AEventAbstract::AEventAbstract(config),
     _socket(std::move(_soc)),
     session(std::bind(&AEvConnection::_respond_handler, this, std::placeholders::_1, nullptr),
             std::bind(&AEvConnection::_respond_handler, this, std::placeholders::_1, std::placeholders::_2))
{
    // std::cout << "AEvConnection CONSTRUCTOR! " << std::endl;
}

void AEvConnection::_ev_begin()
{
    std::cout << "AEvConnection START" << std::endl;
    session.init_async(_gen_conf_for_util());
    session.begin(_socket.remote_endpoint().address().to_string());
    _read_command();
}

void AEvConnection::_ev_finish()
{
    std::cout << "AEvConnection FINISH" << std::endl;
    session.stop_async();
}

void AEvConnection::_ev_stop()
{
    std::cout << "AEvConnection STOP" << std::endl;
    _ev_exit_signal = AEvExitSignal::close_connection;
    _socket.close();
}

void AEvConnection::_ev_timeout()
{
    std::cout << "AEvConnection TIMEOUT" << std::endl;
}

void AEvConnection::_ev_child_callback(AEvPtrBase child_ptr, AEvExitSignal &_ret)
{

}

void AEvConnection::_read_command()
{
    read_cmd_buffer.release(1024);

    _socket.async_read_some(asio::buffer(read_cmd_buffer.data_top(), read_cmd_buffer.size_avail()),
                            _ev_loop->wrap([this](std::error_code ec, std::size_t bytes_transferred){

                                if (ec) {
                                    stop();
                                    return;
                                }
                                read_cmd_buffer.accept(bytes_transferred);
                                if (!read_cmd_buffer.is_empty()) {
                                    session.transaction(read_cmd_buffer);
                                    read_cmd_buffer.mem_reduce();
                                }
                                if (!session.read_data_demand()) {
                                    _read_command();
                                }
                                else {
                                    read_data_buffer.clear();
                                    _read_data();
                                }


                            }));

}

void AEvConnection::_read_data()
{
    read_data_buffer.release(70000);

    _socket.async_read_some(asio::buffer(read_data_buffer.data_top(), read_data_buffer.size_avail()),
                            _ev_loop->wrap([this](std::error_code ec, std::size_t bytes_transferred)
    {

                                if (ec) {
                                    stop();
                                    return;
                                }
                                read_data_buffer.accept(bytes_transferred);
                                if (!read_data_buffer.is_redy()) {
                                    _read_data();
                                } else {
                                    session.accept_data(read_data_buffer);
                                    _read_command();
                                }
                            } ));
}



void AEvConnection::_respond_handler(std::string data, ConfirmHendler confirm)
{

    _socket.async_send(asio::buffer(data),
                       _ev_loop->wrap([this, confirm](std::error_code ec, std::size_t bytes_transferred)
    {

        if (ec) {
            stop();
            return;
        }
        if (session.close_demand()) {
            stop();
            return;
        };

        if (confirm != nullptr) {
            confirm(true);
        }
        reset_and_start_timer();
    }));
}

} //namespace


