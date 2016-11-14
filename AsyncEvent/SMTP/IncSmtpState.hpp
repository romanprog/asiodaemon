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
    // Reply on "RCPT TO" command not sended. --- ???
    size_t rcpts_uninited{0};

    std::string curent_reply;
    bool close_conn {false};

    SmtpDataBufferPtr message_data;

    void reset_transaction()
    {
        recipients.reset();
        mailform.reset();
        waiting_for_data = false;
        rcpts_uninited = 0;
        curent_reply.clear();
        message_data.reset();
        message_data = std::make_shared<SmtpDataBuffer>();
    }
};


} // namespace smtp

#endif // INCSMTPSTATE_HPP
