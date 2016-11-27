#include "AEvHdfs.hpp"
#include "HdfsUtils.hpp"
#include "HdfsBuffer.hpp"
#include "../../HUtils/HStrings.hpp"
#include "../../HUtils/HNet.hpp"

#include "iostream"

namespace aev {

AEvHdfs::AEvHdfs(AEvChildConf &&config_)
    :AEventAbstract::AEventAbstract(std::move(config_)),
     _socket(_ev_loop->get_io_service(), asio::ip::tcp::endpoint(asio::ip::tcp::v4(), 0))

{
    log_debug("AEvHdfs CONSTRUCTOR! ");
}

AEvHdfs::~AEvHdfs()
{
    log_debug("~AEvHdfs DESTRUCTOR");
}

void AEvHdfs::connect()
{
    _socket.close();
    _resolve();
}

void AEvHdfs::_ev_finish()
{
    log_debug("AEvHdfs FINISH" );
    _socket.close();
}

void AEvHdfs::_ev_stop()
{
    log_debug("AEvHdfs STOP" );
}

void AEvHdfs::_ev_timeout()
{

}

void aev::AEvHdfs::_ev_child_callback(AEvPtrBase child_ptr, AEvExitSignal &_ret)
{

}

void AEvHdfs::_resolve()
{
    res_query = std::make_shared<asio::ip::tcp::resolver::query>(_endpoint.domain, _endpoint.port);
    resolver_tmp->async_resolve(*res_query, wrap_asio_cb(
                                   [this](const asio::error_code& ec, asio::ip::tcp::resolver::iterator endpoint_iterator) {

        if (ec) {
            _on_error(ec.message());
            return;
        }
        _connect(endpoint_iterator);
    }
    ));
}

void AEvHdfs::_connect(asio::ip::tcp::resolver::iterator endpoint_iterator)
{
    _socket.async_connect(*endpoint_iterator, wrap_asio_cb([this](const asio::error_code& ec) {

        if (ec){
            _on_error(ec.message());
            return;
        }
        _on_connected();

    }));
}

void AEvHdfs::send_request(std::shared_ptr<std::string> query_)
{
    // log_main(*query_);
    _socket.async_send(asio::buffer(query_->data(), query_->size()), wrap_asio_cb([this](const asio::error_code& ec, size_t bytes_transferred) {
        if (ec){
            _on_error(ec.message());
            return;
        }
        BuffPtr rbuffer {std::make_shared<hdfs::HdfsBuffer>()};
        _receiv_respond(std::move(rbuffer));
    }));
}

void AEvHdfs::_receiv_respond(BuffPtr && rbuffer)
{
    rbuffer->release(2048);
    _socket.async_read_some(asio::buffer(rbuffer->data_top(), rbuffer->size_avail()),
                            wrap_asio_cb([this, rbuffer](const asio::error_code& ec, size_t bytes_readed) mutable
    {
        if (ec){
            _on_error(ec.message());
            return;
        }
        rbuffer->accept(bytes_readed);
        if (rbuffer->unaccepted_content_size())
            _receiv_respond(std::move(rbuffer));
        else
            _on_get_respond(std::move(rbuffer));
    } ));
}

AEvHdfsRead::AEvHdfsRead(AEvChildConf &&config_, hdfs::Target ep_, HdfsReadCb cb_)
    : AEvHdfs::AEvHdfs(std::move(config_)),
      _target(std::move(ep_)),
      _cb(cb_)
{

}

void AEvHdfsRead::_ev_begin()
{
    // Check uri.
    if (!hdfs::utils::parse_url(_target.name_node_uri, _endpoint)) {
        _on_error("Bad uri.");
        return;
    }
    connect();
}

void AEvHdfsRead::_on_connected()
{
    if (_name_node_in_process)
        _get_data_node_loc();
    else
        _get_file();
}

void AEvHdfsRead::_on_get_respond(BuffPtr buffer_)
{
    if (buffer_->have_error()) {
        _on_error("Parser: Server respond error.");
        return;
    }
    // Redirect.
    if (buffer_->get_resp_code() == 307) {
        if (!_name_node_in_process) {
            _cb(true, "Sync error", nullptr);
            return;
        }

        if (!hdfs::utils::parse_location(buffer_->get_redirect_location(), _file_location)) {
            _cb(true, "Redirect url error", nullptr);
            return;
        }
        _endpoint = _file_location.endpoint;
        _name_node_in_process = false;
        connect();
        return;
    }

    _cb(false, "", std::move(buffer_));

    stop();
}

void AEvHdfsRead::_on_error(const std::string error_message_)
{
    log_debug(error_message_);
    _cb(true, error_message_, nullptr);
    stop();
}

void AEvHdfsRead::_get_data_node_loc()
{
    std::shared_ptr<std::string> request{std::make_shared<std::string>("GET ")};
    request->append("/webhdfs/v1");
    request->append(_target.filename);
    request->append("?user.name=");
    request->append(_target.username);
    request->append("&op=OPEN HTTP/1.1\r\nhost: ");
    request->append(_endpoint.domain);
    request->append("\r\n\r\n");
    send_request(request);
}

void AEvHdfsRead::_get_file()
{
    std::shared_ptr<std::string> request{std::make_shared<std::string>("GET ")};
    request->append(_file_location.query);
    request->append(" HTTP/1.1\r\nhost: ");
    request->append(_file_location.endpoint.domain);
    request->append("\r\n\r\n");
    send_request(request);
}

} // namespace aev
