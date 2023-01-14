#include "Player.h"

#include "mod_learnspells.h"

void LearnSpells::OnLevelChanged(Player* player, uint8 /*oldLevel*/)
{
    LearnAllSpells(player);
}

void LearnSpells::OnLogin(Player* player)
{
    LearnAllSpells(player);
}

void LearnSpells::OnPlayerLearnTalents(Player* player, uint32 /*talentId*/, uint32 /*talentRank*/, uint32 /*spellid*/)
{
    LearnAllSpells(player);
}

void LearnSpells::LearnAllSpells(Player* player)
{
    if (player->IsGameMaster() && !EnableGamemasters)
        return;

    LearnClassSpells(player);
    LearnTalentRanks(player);
    LearnProficiencies(player);
    LearnMounts(player);
    AddTotems(player);
}

void LearnSpells::LearnClassSpells(Player* player)
{
    if (!EnableClassSpells && !EnableFromQuests)
        return;

    for (auto& classSpell : ListClassSpells)
    {
        if (classSpell.RequiresQuest == 0 && !EnableClassSpells)
            continue;

        if (classSpell.RequiresQuest == 1 && !EnableFromQuests)
            continue;

        if (classSpell.RaceId == -1 || classSpell.RaceId == player->getRace())
            if (classSpell.ClassId == player->getClass())
                if (player->GetLevel() >= classSpell.RequiredLevel)
                    if (classSpell.RequiredSpellId == -1 || player->HasSpell(classSpell.RequiredSpellId))
                        if (!player->HasSpell(classSpell.SpellId))
                            player->learnSpell(classSpell.SpellId);
    }
}

void LearnSpells::LearnTalentRanks(Player* player)
{
    if (!EnableTalentRanks)
        return;

    for (auto& talentRank : ListTalentRanks)
    {
        if (talentRank.ClassId == player->getClass())
            if (player->GetLevel() >= talentRank.RequiredLevel)
                if (player->HasSpell(talentRank.RequiredSpellId))
                    if (!player->HasSpell(talentRank.SpellId))
                        player->learnSpell(talentRank.SpellId);
    }
}

void LearnSpells::LearnProficiencies(Player* player)
{
    if (!EnableProficiencies)
        return;

    for (auto& proficiency : ListProficiencies)
    {
        if (proficiency.ClassId == player->getClass())
            if (player->GetLevel() >= proficiency.RequiredLevel)
                if (!player->HasSpell(proficiency.SpellId))
                    player->learnSpell(proficiency.SpellId);
    }
}

void LearnSpells::LearnMounts(Player* player)
{
    if (!EnableApprenticeRiding && !EnableJourneymanRiding && !EnableExpertRiding && !EnableArtisanRiding && !EnableColdWeatherFlying)
        return;

    for (auto& mount : ListMounts)
    {
        if ((mount.SpellId == 33388 && !EnableApprenticeRiding) ||
            (mount.SpellId == 33391 && !EnableJourneymanRiding) ||
            (mount.SpellId == 34090 && !EnableExpertRiding) ||
            (mount.SpellId == 34091 && !EnableArtisanRiding) ||
            (mount.SpellId == 54197 && !EnableColdWeatherFlying) ||
            (mount.RequiresQuest == 1 && !EnableFromQuests))
            continue;

        if (mount.RaceId == -1 || mount.RaceId == player->getRace())
            if (mount.ClassId == -1 || mount.ClassId == player->getClass())
                if (mount.TeamId == -1 || mount.TeamId == player->GetTeamId())
                    if (mount.RequiredSpellId == -1 || player->HasSpell(mount.RequiredSpellId))
                        if (player->GetLevel() >= mount.RequiredLevel)
                            if (!player->HasSpell(mount.SpellId))
                                player->learnSpell(mount.SpellId);
    }
}

void LearnSpells::AddTotems(Player* player)
{
    if (player->getClass() != CLASS_SHAMAN)
        return;

    if (!EnableFromQuests)
        return;

    uint32 totems[4][3] = {
        {5175, 2, 4}, // Earth Totem, TotemCategory 2, Level 4
        {5176, 4, 10}, // Fire Totem, TotemCategory 4, Level 10
        {5177, 5, 20}, // Water Totem, TotemCategory 5, Level 20
        {5178, 3, 30} // Air Totem, TotemCategory 3, Level 30
    };

    for (int i = 0; i <= 3; i++)
    {
        if (player->GetLevel() >= totems[i][2])
            if (!player->HasItemTotemCategory(totems[i][1]))
                player->AddItem(totems[i][0], 1);
    }
}
