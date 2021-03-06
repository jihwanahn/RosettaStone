// This code is based on Sabberstone project.
// Copyright (c) 2017-2019 SabberStone Team, darkfriend77 & rnilva
// RosettaStone is hearthstone simulator using C++ with reinforcement learning.
// Copyright (c) 2019 Chris Ohk, Youngjoong Kim, SeungHyun Jeon

#include <Rosetta/PlayMode/Cards/Cards.hpp>
#include <Rosetta/PlayMode/Games/Game.hpp>
#include <Rosetta/PlayMode/Tasks/SimpleTasks/RandomSpellTask.hpp>

#include <effolkronium/random.hpp>

using Random = effolkronium::random_static;

namespace RosettaStone::PlayMode::SimpleTasks
{
RandomSpellTask::RandomSpellTask(CardClass cardClass, GameTag tag, int value,
                                 int amount, RelaSign relaSign, bool opposite)
    : m_cardClass(cardClass),
      m_gameTag(tag),
      m_value(value),
      m_amount(amount),
      m_relaSign(relaSign),
      m_opposite(opposite)
{
    // Do nothing
}

TaskStatus RandomSpellTask::Impl(Player* player)
{
    std::vector<Card*> result;

    if (m_cardClass == CardClass::INVALID)
    {
        const auto cards =
            m_source->game->GetFormatType() == FormatType::STANDARD
                ? Cards::GetAllStandardCards()
                : Cards::GetAllWildCards();

        for (const auto& card : cards)
        {
            if (Evaluate(card))
            {
                result.emplace_back(card);
            }
        }
    }
    else if (m_cardClass == CardClass::PLAYER_CLASS)
    {
        const auto playerClass = player->GetHero()->card->GetCardClass();
        const auto cards =
            m_source->game->GetFormatType() == FormatType::STANDARD
                ? Cards::GetStandardCards(playerClass)
                : Cards::GetWildCards(playerClass);

        for (const auto& card : cards)
        {
            if (Evaluate(card))
            {
                result.emplace_back(card);
            }
        }
    }
    else
    {
        const auto cards =
            m_source->game->GetFormatType() == FormatType::STANDARD
                ? Cards::GetStandardCards(m_cardClass)
                : Cards::GetWildCards(m_cardClass);

        for (const auto& card : cards)
        {
            if (Evaluate(card))
            {
                result.emplace_back(card);
            }
        }
    }

    if (result.empty())
    {
        return TaskStatus::STOP;
    }

    std::vector<Playable*> randomMinions;
    randomMinions.reserve(m_amount);

    if (m_amount > 1)
    {
        std::vector<Card*> list = result;

        while (randomMinions.size() < static_cast<std::size_t>(m_amount) &&
               !result.empty())
        {
            const auto idx = Random::get<std::size_t>(0, list.size() - 1);
            auto card = Entity::GetFromCard(
                m_opposite ? player->opponent : player, list.at(idx));

            list.erase(list.begin() + idx);

            randomMinions.emplace_back(card);
        }
    }
    else
    {
        const auto idx = Random::get<std::size_t>(0, result.size() - 1);
        auto card = Entity::GetFromCard(m_opposite ? player->opponent : player,
                                        result.at(idx));
        randomMinions.emplace_back(card);
    }

    player->game->taskStack.playables = randomMinions;

    return TaskStatus::COMPLETE;
}

std::unique_ptr<ITask> RandomSpellTask::CloneImpl()
{
    return std::make_unique<RandomSpellTask>(m_cardClass, m_gameTag, m_value,
                                             m_amount, m_relaSign, m_opposite);
}

bool RandomSpellTask::Evaluate(Card* card) const
{
    if (card->GetCardType() == CardType::SPELL &&
        ((m_relaSign == RelaSign::EQ && card->gameTags[m_gameTag] == m_value) ||
         (m_relaSign == RelaSign::GEQ &&
          card->gameTags[m_gameTag] >= m_value) ||
         (m_relaSign == RelaSign::LEQ && card->gameTags[m_gameTag] <= m_value)))
    {
        return true;
    }

    return false;
}
}  // namespace RosettaStone::PlayMode::SimpleTasks
