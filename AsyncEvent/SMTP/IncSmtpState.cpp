#include "IncSmtpState.hpp"

namespace smtp {

IncSmtpState::IncSmtpState()
    : message_data(std::make_shared<SmtpDataBuffer>())
{
}

void IncSmtpState::reset_transaction()
{
    recipients.reset();
    mailform.reset();
    waiting_for_data = false;
    curent_reply.clear();
    message_data.reset();
    message_data = std::make_shared<SmtpDataBuffer>();
}



} // namespace smtp
