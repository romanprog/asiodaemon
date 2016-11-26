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
    explicit AEvHdfs(AEvChildConf &&config_);
    ~AEvHdfs();

protected:
    // Try to connect to hdfs name server. @url_ is full http url.
    // Must contain proto://domain:port ("http://example.com:50075")
    void connect(const std::string & url_);
    // Send http query to server. @query_is full http query buffer with all needed headers.
    void send_request(std::shared_ptr<std::string> query_);
    // Calls when connected.
    virtual void _on_connected() = 0;
    // Calls, when full server respond received. Buffer as is.
    virtual void _on_get_respond(BuffPtr buffer_) = 0;
    // Calls when any error occur.
    virtual void _on_error(const std::string error_message_) = 0;
    std::shared_ptr<std::string> _full_query_ptr;
    hdfs::PUri _parsed_url;

private:
    asio::ip::tcp::socket _socket;
    std::shared_ptr<asio::ip::tcp::resolver> resolver_tmp {std::make_shared<asio::ip::tcp::resolver>(_ev_loop->get_io_service())};
    std::shared_ptr<asio::ip::tcp::resolver::query> res_query;

    virtual void _ev_finish() override;
    virtual void _ev_stop() override;
    virtual void _ev_timeout() override;
    virtual void _ev_child_callback(AEvPtrBase child_ptr, AEvExitSignal & _ret) override;

    void _resolve();
    void _connect(asio::ip::tcp::resolver::iterator endpoint_iterator);
    void _receiv_respond(BuffPtr && rbuffer);

};

/// ////////////////////////////////////////////////////////////////////////////////////
/// Read file query.
///
/// ////////////////////////////////////////////////////////////////////////////////////

using HdfsReadCb = std::function<void(bool er_, std::string er_msg_, BuffPtr && data_)>;
class AEvHdfsRead : public AEvHdfs
{
public:
    AEvHdfsRead(AEvChildConf &&config_, hdfs::Target ep_, HdfsReadCb cb_);
private:
    virtual void _ev_begin() override;
    virtual void _on_connected() override;
    virtual void _on_get_respond(BuffPtr buffer_) override;
    virtual void _on_error(const std::string error_message_) override;

    std::string __build_uri(const hdfs::Target & ep_);

    bool _name_node_in_process {true};

    void _conn_to_namenode();
    void _conn_to_data_node();
    void _get_data_node_loc();
    void _get_file();
    hdfs::Target _target;
    HdfsReadCb _cb;
};


} //namespace

#endif // AEVHDFS_HPP
