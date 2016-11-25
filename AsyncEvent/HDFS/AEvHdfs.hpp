#ifndef AEVHDFS_HPP
#define AEVHDFS_HPP

#include "../AEvBase/AEventAbstract.hpp"
#include "HdfsBuffer.hpp"
#include "HdfsTypes.hpp"

/// ///////////////////////////////
/// REST API asinc implementation.
/// ///////////////////////////////
namespace aev {

using BuffPtr = std::shared_ptr<hdfs::HdfsBuffer>;


class AEvHdfs : public AEventAbstract
{
public:
    explicit AEvHdfs(AEvChildConf &&config);
    ~AEvHdfs();

protected:
    // Try to connect to hdfs name server. @url_ is full http url.
    // Must contain proto://domain:port ("http://example.com:50075")
    void connect(const std::string & url_);
    // Send http query to server. @query_is full http query buffer with all needed headers.
    void send_request(std::shared_ptr<std::string> query_);
    // Calls when connected (or have any error).
    virtual void _on_connected(bool have_error_, const std::string error_message_) = 0;
    // Calls, when full server respond received. Buffer as is.
    virtual void _on_get_respond(asio::error_code ec_, BuffPtr buffer_) = 0;

private:
    asio::ip::tcp::socket _socket;
    std::shared_ptr<std::string> _full_query_ptr;
    std::string _proto; // "http" or "https"
    std::string _domain; // FQDN.
    std::string _port; // Remote host port number in text format.

    virtual void _ev_finish() override;
    virtual void _ev_stop() override;
    virtual void _ev_timeout() override;
    virtual void _ev_child_callback(AEvPtrBase child_ptr, AEvExitSignal & _ret) override;

    void _resolve();
    void _connect(asio::ip::tcp::resolver::iterator endpoint_iterator);


};

} //namespace

#endif // AEVHDFS_HPP
