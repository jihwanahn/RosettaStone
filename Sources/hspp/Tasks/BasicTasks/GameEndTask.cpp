// Copyright (c) 2018 Chris Ohk, Youngjoong Kim, SeungHyun Jeon

// We are making my contributions/submissions to this project solely in our
// personal capacity and are not conveying any rights to any intellectual
// property of any third parties.

#include <hspp/Tasks/BasicTasks/GameEndTask.hpp>

namespace Hearthstonepp::BasicTasks
{
TaskID GameEndTask::GetTaskID() const
{
    return TaskID::GAME_END;
}

MetaData GameEndTask::Impl(Player&)
{
    return MetaData::GAME_END;
}
}  // namespace Hearthstonepp::BasicTasks