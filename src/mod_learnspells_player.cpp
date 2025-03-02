#include "Player.h"

#include "mod_learnspells.h"

#include "mod_progression.h"

void LearnSpells::OnPlayerLevelChanged(Player* player, uint8 /*oldLevel*/)
{
    LearnAllSpells(player);
}

void LearnSpells::OnPlayerLogin(Player* player)
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
    {
        return;
    }

    if (player->getClass() == CLASS_DEATH_KNIGHT && player->GetMapId() == 609)
    {
        return;
    }

    LearnClassSpells(player);
    LearnTalentRanks(player);
    LearnProficiencies(player);
    LearnMounts(player);
    AddTotems(player);
}

void LearnSpells::LearnClassSpells(Player* player)
{
    if (!EnableClassSpells && !EnableFromQuests)
    {
        return;
    }

    std::vector<std::vector<int>> spells = GetSpells(TYPE_CLASS);

    if (spells.empty())
    {
        return;
    }

    for (auto& spell : spells)
    {
        if (spell[SPELL_REQUIRES_QUEST] == 0 && !EnableClassSpells)
        {
            continue;
        }

        if (spell[SPELL_REQUIRES_QUEST] == 1 && !EnableFromQuests)
        {
            continue;
        }

        if (sProgressionMgr->GetPatchId() < 19 && (spell[SPELL_ID] == 33950 || spell[SPELL_ID] == 66842 || spell[SPELL_ID] == 66843 || spell[SPELL_ID] == 66844))
        {
            continue;
        }

        if (sProgressionMgr->GetPatchId() < 17 && (spell[SPELL_ID] == 1066 || spell[SPELL_ID] == 8946 || spell[SPELL_ID] == 23922 ||
                                                spell[SPELL_ID] == 25392 || spell[SPELL_ID] == 26991 || spell[SPELL_ID] == 33717 ||
                                                spell[SPELL_ID] == 39374 || spell[SPELL_ID] == 62124))
        {
            continue;
        }

        if (sProgressionMgr->GetPatchId() < 12 && (spell[SPELL_ID] == 21562 || spell[SPELL_ID] == 21564 || spell[SPELL_ID] == 21849 || spell[SPELL_ID] == 21850 ||
                                                spell[SPELL_ID] == 23028 || spell[SPELL_ID] == 25286 || spell[SPELL_ID] == 25288 || spell[SPELL_ID] == 25289 ||
                                                spell[SPELL_ID] == 25290 || spell[SPELL_ID] == 25291 || spell[SPELL_ID] == 25292 || spell[SPELL_ID] == 25294 ||
                                                spell[SPELL_ID] == 25295 || spell[SPELL_ID] == 25296 || spell[SPELL_ID] == 25297 || spell[SPELL_ID] == 25298 ||
                                                spell[SPELL_ID] == 25299 || spell[SPELL_ID] == 25300 || spell[SPELL_ID] == 25302 || spell[SPELL_ID] == 25304 ||
                                                spell[SPELL_ID] == 25306 || spell[SPELL_ID] == 25307 || spell[SPELL_ID] == 25309 || spell[SPELL_ID] == 25311 ||
                                                spell[SPELL_ID] == 25314 || spell[SPELL_ID] == 25315 || spell[SPELL_ID] == 25316 || spell[SPELL_ID] == 25345 ||
                                                spell[SPELL_ID] == 25357 || spell[SPELL_ID] == 25361 || spell[SPELL_ID] == 27090 || spell[SPELL_ID] == 27127 ||
                                                spell[SPELL_ID] == 27683 || spell[SPELL_ID] == 28609 || spell[SPELL_ID] == 28610 || spell[SPELL_ID] == 28612 ||
                                                spell[SPELL_ID] == 29228 || spell[SPELL_ID] == 31016 || spell[SPELL_ID] == 31018 || spell[SPELL_ID] == 31709 ||
                                                spell[SPELL_ID] == 36936 || spell[SPELL_ID] == 31789))
        {
            continue;
        }

        if (sProgressionMgr->GetPatchId() < 8 && (spell[SPELL_ID] == 25782 || spell[SPELL_ID] == 25894 || spell[SPELL_ID] == 25898 || spell[SPELL_ID] == 25899 ||
                                               spell[SPELL_ID] == 25916 || spell[SPELL_ID] == 25918))
        {
            continue;
        }

        if (spell[SPELL_REQUIRED_RACE] == -1 || spell[SPELL_REQUIRED_RACE] == player->getRace())
        {
            if (spell[SPELL_REQUIRED_CLASS] == player->getClass())
            {
                if (player->GetLevel() >= spell[SPELL_REQUIRED_LEVEL])
                {
                    if (spell[SPELL_REQUIRED_SPELL_ID] == -1 || player->HasSpell(spell[SPELL_REQUIRED_SPELL_ID]))
                    {
                        if (!player->HasSpell(spell[SPELL_ID]))
                        {
                            player->learnSpell(spell[SPELL_ID]);
                        }
                    }
                }
            }
        }
    }
}

void LearnSpells::LearnTalentRanks(Player* player)
{
    if (!EnableTalentRanks)
    {
        return;
    }

    std::vector<std::vector<int>> spells = GetSpells(TYPE_TALENTS);

    if (spells.empty())
    {
        return;
    }

    for (auto& spell : spells)
    {
        if (spell[SPELL_REQUIRED_CLASS] == player->getClass())
        {
            if (player->GetLevel() >= spell[SPELL_REQUIRED_LEVEL])
            {
                if (player->HasSpell(spell[SPELL_REQUIRED_SPELL_ID]))
                {
                    if (!player->HasSpell(spell[SPELL_ID]))
                    {
                        player->learnSpell(spell[SPELL_ID]);
                    }
                }
            }
        }
    }
}

void LearnSpells::LearnProficiencies(Player* player)
{
    if (!EnableProficiencies)
    {
        return;
    }

    std::vector<std::vector<int>> spells = GetSpells(TYPE_PROFICIENCIES);

    if (spells.empty())
    {
        return;
    }

    for (auto& spell : spells)
    {
        if (spell[SPELL_REQUIRED_CLASS] == player->getClass())
        {
            if (player->GetLevel() >= spell[SPELL_REQUIRED_LEVEL])
            {
                if (!player->HasSpell(spell[SPELL_ID]))
                {
                    player->learnSpell(spell[SPELL_ID]);
                }
            }
        }
    }
}

void LearnSpells::LearnMounts(Player* player)
{
    if (!EnableApprenticeRiding && !EnableJourneymanRiding && !EnableExpertRiding && !EnableArtisanRiding && !EnableColdWeatherFlying)
    {
        return;
    }

    std::vector<std::vector<int>> spells = GetSpells(TYPE_MOUNTS);

    if (spells.empty())
    {
        return;
    }

    for (auto& spell : spells)
    {
        if (((spell[SPELL_ID] == SPELL_APPRENTICE_RIDING || spell[SPELL_REQUIRED_SPELL_ID] == SPELL_APPRENTICE_RIDING) && !EnableApprenticeRiding) ||
            ((spell[SPELL_ID] == SPELL_JOURNEYMAN_RIDING || spell[SPELL_REQUIRED_SPELL_ID] == SPELL_JOURNEYMAN_RIDING) && !EnableJourneymanRiding) ||
            ((spell[SPELL_ID] == SPELL_EXPERT_RIDING || spell[SPELL_REQUIRED_SPELL_ID] == SPELL_EXPERT_RIDING) && !EnableExpertRiding) ||
            ((spell[SPELL_ID] == SPELL_ARTISAN_RIDING || spell[SPELL_REQUIRED_SPELL_ID] == SPELL_ARTISAN_RIDING) && !EnableArtisanRiding) ||
            (spell[SPELL_ID] == SPELL_COLD_WEATHER_FLYING && !EnableColdWeatherFlying) ||
            (spell[SPELL_REQUIRES_QUEST] == 1 && !EnableFromQuests))
        {
            continue;
        }

        if (sProgressionMgr->GetPatchId() < 17)
        {
            if ((spell[SPELL_REQUIRED_SPELL_ID] == SPELL_APPRENTICE_RIDING && spell[SPELL_ID] != SPELL_JOURNEYMAN_RIDING)
            || (spell[SPELL_REQUIRED_SPELL_ID] == SPELL_JOURNEYMAN_RIDING && spell[SPELL_ID] != SPELL_EXPERT_RIDING)
            || (spell[SPELL_REQUIRED_SPELL_ID] == SPELL_EXPERT_RIDING && spell[SPELL_ID] != SPELL_ARTISAN_RIDING))
            {
                continue;
            }
        }

        if (sProgressionMgr->GetPatchId() < 19)
        {
            if (spell[SPELL_ID] == SPELL_APPRENTICE_RIDING)
            {
                spell[SPELL_REQUIRED_LEVEL] = sProgressionMgr->GetPatchId() < 16 ? 40 : 30;
            }
            else if (spell[SPELL_ID] == SPELL_JOURNEYMAN_RIDING)
            {
                spell[SPELL_REQUIRED_LEVEL] = 60;
            }
            else if (spell[SPELL_ID] == SPELL_EXPERT_RIDING)
            {
                spell[SPELL_REQUIRED_LEVEL] = 70;
            }
            else if (spell[SPELL_REQUIRED_SPELL_ID] == SPELL_APPRENTICE_RIDING && spell[SPELL_ID] != SPELL_JOURNEYMAN_RIDING)
            {
                spell[SPELL_REQUIRED_LEVEL] = sProgressionMgr->GetPatchId() < 16 ? 40 : 30;
            }
            else if (spell[SPELL_REQUIRED_SPELL_ID] == SPELL_JOURNEYMAN_RIDING && spell[SPELL_ID] != SPELL_EXPERT_RIDING)
            {
                spell[SPELL_REQUIRED_LEVEL] = 60;
            }
            else if (spell[SPELL_REQUIRED_SPELL_ID] == SPELL_EXPERT_RIDING && (spell[SPELL_ID] != SPELL_ARTISAN_RIDING && spell[SPELL_ID] != SPELL_COLD_WEATHER_FLYING))
            {
                spell[SPELL_REQUIRED_LEVEL] = 70;
            }
        }

        if (spell[SPELL_REQUIRED_RACE] == -1 || spell[SPELL_REQUIRED_RACE] == player->getRace())
        {
            if (spell[SPELL_REQUIRED_CLASS] == -1 || spell[SPELL_REQUIRED_CLASS] == player->getClass())
            {
                if (spell[SPELL_REQUIRED_TEAM] == -1 || spell[SPELL_REQUIRED_TEAM] == player->GetTeamId())
                {
                    if (spell[SPELL_REQUIRED_SPELL_ID] == -1 || player->HasSpell(spell[SPELL_REQUIRED_SPELL_ID]))
                    {
                        if (player->GetLevel() >= spell[SPELL_REQUIRED_LEVEL])
                        {
                            if (!player->HasSpell(spell[SPELL_ID]))
                            {
                                player->learnSpell(spell[SPELL_ID]);
                            }
                        }
                    }
                }
            }
        }
    }
}

void LearnSpells::AddTotems(Player* player)
{
    if (player->getClass() != CLASS_SHAMAN)
    {
        return;
    }

    if (!EnableClassSpells || !EnableFromQuests)
    {
        return;
    }

    uint32 totems[4][3] =
    {
        {5175, 2, 4}, // Earth Totem, TotemCategory 2, Level 4
        {5176, 4, 10}, // Fire Totem, TotemCategory 4, Level 10
        {5177, 5, 20}, // Water Totem, TotemCategory 5, Level 20
        {5178, 3, 30} // Air Totem, TotemCategory 3, Level 30
    };

    for (int i = 0; i <= 3; i++)
    {
        if (player->GetLevel() >= totems[i][2])
        {
            if (!player->HasItemTotemCategory(totems[i][1]))
            {
                player->AddItem(totems[i][0], 1);
            }
        }
    }
}
