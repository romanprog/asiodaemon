#include "BaseMod.hpp"

namespace smtp {


BaseMod::BaseMod(SmtpStatePtr state_, CommandsMap &cmd_map_)
    : _ProtoState(std::move(state_))
{
    _RegisterCommands(cmd_map_);
}

void BaseMod::_RegisterCommands(CommandsMap &cmd_map_)
{

}


} // namespace smtp
