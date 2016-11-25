#include "AEvHdfs.hpp"
#include "HdfsBuffer.hpp"
#include "../../HUtils/HStrings.hpp"
#include "../../HUtils/HNet.hpp"

#include "iostream"

namespace aev {

AEvHdfs::AEvHdfs(AEvChildConf &&config)
    :AEventAbstract::AEventAbstract(std::move(config)),
     _socket(_ev_loop->get_io_service(), asio::ip::tcp::endpoint(asio::ip::tcp::v4(), 0))

{
    log_debug("AEvHdfs CONSTRUCTOR! ");
}

AEvHdfs::~AEvHdfs()
{
    log_debug("~AEvHdfs DESTRUCTOR");
}

void AEvHdfs::connect(const std::string &url_)
{
    std::vector<std::string> splitted_uri_ = hstrings::splitted(url_,':');
    if (splitted_uri_.size() != 3) {
        _on_connected(true, "Bad uri.");
        return;
    }
    if (splitted_uri_[1][0] != '/' || splitted_uri_[1][1] != '/') {
        _on_connected(true, "Bad uri.");
        return;
    }
    if (!hstrings::is_digit_only(splitted_uri_[2])) {
        _on_connected(true, "Bad port.");
        return;
    }
    if (!hnet::is_fqdn(splitted_uri_[1])) {
        _on_connected(true, "Bad domain.");
        return;
    }
    _proto = std::move(splitted_uri_[0]);
    _domain = std::move(splitted_uri_[1]);
    // trim "//" from begining of domain.
    _domain.erase(_domain.begin(), _domain.begin() + 2);
    _port = std::move(splitted_uri_[2]);


}

void AEvHdfs::_ev_finish()
{
    log_debug("AEvHdfs FINISH" );
    _socket.close();
}

void AEvHdfs::_ev_stop()
{

}

void AEvHdfs::_ev_timeout()
{

}

// void AEvHdfs::_send_request()
// {

//    asio::ip::tcp::resolver resolver(_ev_loop->get_io_service());
//    endpoint = *resolver.resolve({asio::ip::tcp::v4(), "127.0.1.1", "53"});
//    // resolver.async_resolve()

//    if (!buff.prepare_for_request(_domain, query_type))
//    {
//        stop();
//        return;
//    }
//    _socket.async_send_to(asio::buffer(buff.data(), buff.size_filled()), endpoint,
//                          wrap_callback([this](std::error_code ec, std::size_t bytes_sent)
//    {
//                              if (ec) {
//                                  stop();
//                                  return;
//                              }
//                              log_debug("sended DNS");
//                              _get_respond();
//                          })
//            );
//}

//void AEvHdfs::_get_respond()
//{

////        if (!buff.prepare_for_respond())
////        {
////            stop();
////            return;
////        }
////        _socket.async_receive_from(asio::buffer(buff.data_top(), buff.size_avail()), endpoint,
////                                   wrap_callback([this](std::error_code ec, std::size_t bytes_sent)
////        {
////                                       if (ec) {
////                                           stop();
////                                           return;
////                                       }
////                                       log_debug("receive DNS");
////                                       buff.read_respond(bytes_sent);
////                                       err = buff.get_error();
////                                       stop();
////                                   })
////                );

//}

void aev::AEvHdfs::_ev_child_callback(AEvPtrBase child_ptr, AEvExitSignal &_ret)
{

}

void AEvHdfs::_resolve()
{
    asio::ip::tcp::resolver resolver_tmp{_ev_loop->get_io_service()};
    asio::ip::tcp::resolver::query res_query{_domain, _port};
    resolver_tmp.async_resolve(res_query, wrap_callback(
                                   [this](const asio::error_code& ec, asio::ip::tcp::resolver::iterator endpoint_iterator)
                                    {
                                        if (ec) {
                                            _on_connected(true, ec.message());
                                            return;
                                        }
                                        _connect(endpoint_iterator);
                                    }
    ));
}

void AEvHdfs::_connect(asio::ip::tcp::resolver::iterator endpoint_iterator)
{

}

void AEvHdfs::send_request(std::shared_ptr<std::string> query_)
{

}


} // namespace aev
