// This code is based on Sabberstone project.
// Copyright (c) 2017-2019 SabberStone Team, darkfriend77 & rnilva
// RosettaStone is hearthstone simulator using C++ with reinforcement learning.
// Copyright (c) 2019 Chris Ohk, Youngjoong Kim, SeungHyun Jeon

#include <Rosetta/Actions/Draw.hpp>
#include <Rosetta/Actions/Generic.hpp>
#include <Rosetta/Models/Player.hpp>
#include <Rosetta/Tasks/ITask.hpp>
#include <Rosetta/Zones/DeckZone.hpp>
#include <Rosetta/Zones/HandZone.hpp>
#include <Rosetta/Zones/SetasideZone.hpp>

namespace RosettaStone::Generic
{
Playable* Draw(Player* player, Playable* cardToDraw)
{
    // Take fatigue damage for player if deck is empty
    if (player->GetDeckZone()->IsEmpty())
    {
        const int fatigueDamage = player->GetHero()->fatigue + 1;
        player->GetHero()->TakeDamage(player->GetHero(), fatigueDamage);

        return nullptr;
    }

    // Get card to draw
    Playable* playable = player->GetDeckZone()->Remove(
        cardToDraw != nullptr ? cardToDraw
                              : player->GetDeckZone()->GetTopCard());

    // Add card to hand
    if (AddCardToHand(player, playable))
    {
        auto tasks = playable->card->power.GetTopdeckTask();

        // Process topdeck tasks
        if (!tasks.empty())
        {
            player->GetSetasideZone()->Add(
                player->GetHandZone()->Remove(playable));

            for (auto& task : tasks)
            {
                std::unique_ptr<ITask> clonedTask = task->Clone();

                clonedTask->SetPlayer(player);
                clonedTask->SetSource(playable);
                clonedTask->SetTarget(nullptr);

                clonedTask->Run();
            }

            Draw(player, cardToDraw);
        }
    }

    return playable;
}

Playable* DrawCard(Player* player, Card* card)
{
    Playable* playable = Entity::GetFromCard(player, card);
    AddCardToHand(player, playable);

    return playable;
}
}  // namespace RosettaStone::Generic
