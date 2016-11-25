#ifndef INCSMTPSTATE_HPP
#define INCSMTPSTATE_HPP

#include "SmtpTypes.hpp"
#include "SmtpBuffer.hpp"

using SmtpDataBufferPtr = std::shared_ptr<SmtpDataBuffer>;

namespace smtp {

struct IncSmtpState
{
public:
    IncSmtpState();
    smtp::EmailRcpts recipients;
    smtp::EmailAddr mailform;
    smtp::HeloArg client_hello;
    std::string client_ip;
    std::string client_ip_ptr;
    bool waiting_for_data{false};
    std::string curent_reply;
    bool close_conn {false};

    SmtpDataBufferPtr message_data;

    void reset_transaction();
};


} // namespace smtp

#endif // INCSMTPSTATE_HPP
