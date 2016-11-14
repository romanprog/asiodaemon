#include "IncSmtpState.hpp"

namespace smtp {

IncSmtpState::IncSmtpState()
    : message_data(std::make_shared<SmtpDataBuffer>())
{
}



} // namespace smtp
