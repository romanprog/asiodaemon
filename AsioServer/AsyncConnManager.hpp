#ifndef ASYNCCONNMANAGER_HPP
#define ASYNCCONNMANAGER_HPP
#include "AsyncConnection.hpp"
#include "AEventsTypes.hpp"

#include <iostream>
#include <functional>
#include <memory>
#include <asio.hpp>
#include <set>

using ChainCallback = std::function<int (int, int)>;
using StrandPtr = std::shared_ptr<asio::strand>;

struct EvChainConf
{
    StrandPtr Loop;
    ChainCallback FinishCallback;
    unsigned Timeout = 0;
    unsigned Id;
};

class AsyncConnManager
{
public:


    using ConnPtr = std::shared_ptr<AsyncConnection>;

    AsyncConnManager(StrandPtr strand, const std::string & ip, const unsigned port);
    void stop();
    void async_start();

private:
    // Delete copy constructor.
    AsyncConnManager(AsyncConnManager &&) = delete;
    AsyncConnManager & operator= (AsyncConnManager &&) = delete;

    int todo1; // TODO int command must be enum type.
    int child_callback(int chID, int command);

    StrandPtr _ev_loop;
    asio::deadline_timer _ev_timeout;
    bool _stoped {false};
    bool _have_error {false};
    std::string _conn_ip;
    unsigned _conn_port;
    asio::ip::tcp::socket _socket;
    asio::ip::tcp::acceptor _acceptor;
    std::vector<ConnPtr> _connections;

    aev::AEvRootConf as;

    void start_accept_();
};

#endif // ASYNCCONNMANAGER_HPP
