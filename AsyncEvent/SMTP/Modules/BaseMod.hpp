#ifndef BASEMOD
#define BASEMOD

#include "../SmtpTypes.hpp"

namespace smtp {

class BaseMod
{
public:
    explicit BaseMod(smtp::SmtpStatePtr state_, smtp::CommandsMap & cmd_map_);

private:
    // Modifiable SMTP state (inside one connection).
    SmtpStatePtr _ProtoState;
    void _RegisterCommands(smtp::CommandsMap & cmd_map_);

};

} // namespace smtp

#endif // BASEMOD
