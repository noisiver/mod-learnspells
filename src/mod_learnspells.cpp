#include "Config.h"
#include "Player.h"
#include "ScriptMgr.h"

struct ClassSpells
{
    int RaceId;
    int ClassId;
    int SpellId;
    int RequiredLevel;
    int RequiredSpellId;
    int RequiresQuest;
};

struct TalentRanks
{
    int ClassId;
    int SpellId;
    int RequiredLevel;
    int RequiredSpellId;
};

struct Proficiencies
{
    int ClassId;
    int SpellId;
    int RequiredLevel;
};

struct Mounts
{
    int RaceId;
    int ClassId;
    int TeamId;
    int SpellId;
    int RequiredLevel;
    int RequiredSpellId;
    int RequiresQuest;
};

std::vector<ClassSpells> lsClassSpells;
std::vector<TalentRanks> lsTalentRanks;
std::vector<Proficiencies> lsProficiencies;
std::vector<Mounts> lsMounts;

enum SpellType
{
    CLASS = 0,
    TALENT,
    PROFICIENCY,
    MOUNT
};

bool lsEnableGamemasters;
bool lsEnableClassSpells;
bool lsEnableTalentRanks;
bool lsEnableProficiencies;
bool lsEnableFromQuests;
bool lsEnableApprenticeRiding;
bool lsEnableJourneymanRiding;
bool lsEnableExpertRiding;
bool lsEnableArtisanRiding;
bool lsEnableColdWeatherFlying;

class LearnSpells : public PlayerScript
{
public:
    LearnSpells() : PlayerScript("LearnSpells") {}

    void OnPlayerLearnTalents(Player* player, uint32 /*talentId*/, uint32 /*talentRank*/, uint32 /*spellid*/) override
    {
        if (lsEnableTalentRanks)
            if (player->GetSession()->GetSecurity() == SEC_PLAYER || lsEnableGamemasters)
                LearnTalentRanksForNewLevel(player);
    }

    void OnLevelChanged(Player* player, uint8 /*oldLevel*/) override
    {
        if (player->GetSession()->GetSecurity() == SEC_PLAYER || lsEnableGamemasters)
            LearnAllSpells(player);
    }

    void OnLogin(Player* player) override
    {
        if (player->GetSession()->GetSecurity() == SEC_PLAYER || lsEnableGamemasters)
            LearnAllSpells(player);
    }
private:
    void LearnAllSpells(Player* player)
    {
        if (lsEnableClassSpells || lsEnableFromQuests)
            LearnSpellsForNewLevel(player);

        if (lsEnableTalentRanks)
            LearnTalentRanksForNewLevel(player);

        if (lsEnableProficiencies)
            LearnProficienciesForNewLevel(player);

        if (lsEnableApprenticeRiding || lsEnableJourneymanRiding || lsEnableExpertRiding || lsEnableArtisanRiding || lsEnableColdWeatherFlying)
            LearnMountsForNewLevel(player);

        if (lsEnableFromQuests && player->getClass() == CLASS_SHAMAN)
            AddShamanTotems(player);
    }

    void LearnSpellsForNewLevel(Player* player)
    {
        for (auto& classSpell : lsClassSpells)
        {
            if (classSpell.RequiresQuest == 0 && !lsEnableClassSpells)
                continue;

            if (classSpell.RequiresQuest == 1 && !lsEnableFromQuests)
                continue;

            if (classSpell.RaceId == -1 || classSpell.RaceId == player->getRace())
                if (classSpell.ClassId == player->getClass())
                    if (player->getLevel() >= classSpell.RequiredLevel)
                        if (classSpell.RequiredSpellId == -1 || player->HasSpell(classSpell.RequiredSpellId))
                            if (!player->HasSpell(classSpell.SpellId))
                                player->learnSpell(classSpell.SpellId);
        }
    }

    void LearnTalentRanksForNewLevel(Player* player)
    {
        for (auto& talentRank : lsTalentRanks)
        {
            if (talentRank.ClassId == player->getClass())
                if (player->getLevel() >= talentRank.RequiredLevel)
                    if (player->HasSpell(talentRank.RequiredSpellId))
                        if (!player->HasSpell(talentRank.SpellId))
                            player->learnSpell(talentRank.SpellId);
        }
    }

    void LearnProficienciesForNewLevel(Player* player)
    {
        for (auto& proficiency : lsProficiencies)
        {
            if (proficiency.ClassId == player->getClass())
                if (player->getLevel() >= proficiency.RequiredLevel)
                    if (!player->HasSpell(proficiency.SpellId))
                        player->learnSpell(proficiency.SpellId);
        }
    }

    void LearnMountsForNewLevel(Player* player)
    {
        for (auto& mount : lsMounts)
        {
            if ((mount.SpellId == 33388 && !lsEnableApprenticeRiding) ||
                (mount.SpellId == 33391 && !lsEnableJourneymanRiding) ||
                (mount.SpellId == 34090 && !lsEnableExpertRiding) ||
                (mount.SpellId == 34091 && !lsEnableArtisanRiding) ||
                (mount.SpellId == 54197 && !lsEnableColdWeatherFlying) ||
                (mount.RequiresQuest == 1 && !lsEnableFromQuests))
                continue;

            if (mount.RaceId == -1 || mount.RaceId == player->getRace())
                if (mount.ClassId == -1 || mount.ClassId == player->getClass())
                    if (mount.TeamId == -1 || mount.TeamId == player->GetTeamId())
                        if (mount.RequiredSpellId == -1 || player->HasSpell(mount.RequiredSpellId))
                            if (player->getLevel() >= mount.RequiredLevel)
                                if (!player->HasSpell(mount.SpellId))
                                    player->learnSpell(mount.SpellId);
        }
    }

    void AddShamanTotems(Player* player)
    {
        uint32 totems[4][3] = {
            {5175, 2, 4}, // Earth Totem, TotemCategory 2, Level 4
            {5176, 4, 10}, // Fire Totem, TotemCategory 4, Level 10
            {5177, 5, 20}, // Water Totem, TotemCategory 5, Level 20
            {5178, 3, 30} // Air Totem, TotemCategory 3, Level 30
        };

        for (int i = 0; i <= 3; i++)
        {
            if (player->getLevel() >= totems[i][2])
                if (!player->HasItemTotemCategory(totems[i][1]))
                    player->AddItem(totems[i][0], 1);
        }
    }
};

class LearnSpellsData : public WorldScript
{
public:
    LearnSpellsData() : WorldScript("LearnSpellsData") { }

    void OnAfterConfigLoad(bool reload) override
    {
        lsEnableGamemasters = sConfigMgr->GetOption<bool>("LearnSpells.Gamemasters", 0);
        lsEnableClassSpells = sConfigMgr->GetOption<bool>("LearnSpells.ClassSpells", 1);
        lsEnableTalentRanks = sConfigMgr->GetOption<bool>("LearnSpells.TalentRanks", 1);
        lsEnableProficiencies = sConfigMgr->GetOption<bool>("LearnSpells.Proficiencies", 1);
        lsEnableFromQuests = sConfigMgr->GetOption<bool>("LearnSpells.SpellsFromQuests", 1);
        lsEnableApprenticeRiding = sConfigMgr->GetOption<bool>("LearnSpells.Riding.Apprentice", 0);
        lsEnableJourneymanRiding = sConfigMgr->GetOption<bool>("LearnSpells.Riding.Journeyman", 0);
        lsEnableExpertRiding = sConfigMgr->GetOption<bool>("LearnSpells.Riding.Expert", 0);
        lsEnableArtisanRiding = sConfigMgr->GetOption<bool>("LearnSpells.Riding.Artisan", 0);
        lsEnableColdWeatherFlying = sConfigMgr->GetOption<bool>("LearnSpells.Riding.ColdWeatherFlying", 0);

        if (reload)
            LoadSpells();
    }

    void OnStartup() override
    {
        LoadSpells();
    }

private:
    void LoadSpells()
    {
        LOG_INFO("server.loading", "Loading spells...");
        {
            LoadClassSpells();
            LoadTalentRanks();
            LoadProficiencies();
            LoadMounts();
        }
    }

    void LoadClassSpells()
    {
        QueryResult result = WorldDatabase.Query("SELECT `race_id`, `class_id`, `spell_id`, `required_level`, `required_spell_id`, `requires_quest` FROM `mod_learnspells` WHERE `type`={} ORDER BY `id` ASC", SpellType::CLASS);

        if (!result)
        {
            LOG_INFO("server.loading", ">> Loaded 0 class spells. DB table `mod_learnspells` has no spells of type 0.");
            return;
        }

        lsClassSpells.clear();

        int i = 0;
        do
        {
            Field* fields = result->Fetch();

            lsClassSpells.push_back(ClassSpells());
            lsClassSpells[i].RaceId = fields[0].Get<int32>();
            lsClassSpells[i].ClassId = fields[1].Get<int32>();
            lsClassSpells[i].SpellId = fields[2].Get<int32>();
            lsClassSpells[i].RequiredLevel = fields[3].Get<int32>();
            lsClassSpells[i].RequiredSpellId = fields[4].Get<int32>();
            lsClassSpells[i].RequiresQuest = fields[5].Get<int32>();

            i++;
        } while (result->NextRow());

        LOG_INFO("server.loading", ">> Loaded {} class spells", i);
    }

    void LoadTalentRanks()
    {
        QueryResult result = WorldDatabase.Query("SELECT `class_id`, `spell_id`, `required_level`, `required_spell_id` FROM `mod_learnspells` WHERE `type`={} ORDER BY `id` ASC", SpellType::TALENT);

        if (!result)
        {
            LOG_INFO("server.loading", ">> Loaded 0 talent ranks. DB table `mod_learnspells` has no spells of type 1.");
            return;
        }

        lsTalentRanks.clear();

        int i = 0;
        do
        {
            Field* fields = result->Fetch();

            lsTalentRanks.push_back(TalentRanks());
            lsTalentRanks[i].ClassId = fields[0].Get<int32>();
            lsTalentRanks[i].SpellId = fields[1].Get<int32>();
            lsTalentRanks[i].RequiredLevel = fields[2].Get<int32>();
            lsTalentRanks[i].RequiredSpellId = fields[3].Get<int32>();

            i++;
        } while (result->NextRow());

        LOG_INFO("server.loading", ">> Loaded {} talent ranks", i);
    }

    void LoadProficiencies()
    {
        QueryResult result = WorldDatabase.Query("SELECT `class_id`, `spell_id`, `required_level` FROM `mod_learnspells` WHERE `type`={} ORDER BY `id` ASC", SpellType::PROFICIENCY);

        if (!result)
        {
            LOG_INFO("server.loading", ">> Loaded 0 proficiencies. DB table `mod_learnspells` has no spells of type 2.");
            return;
        }

        lsProficiencies.clear();

        int i = 0;
        do
        {
            Field* fields = result->Fetch();

            lsProficiencies.push_back(Proficiencies());
            lsProficiencies[i].ClassId = fields[0].Get<int32>();
            lsProficiencies[i].SpellId = fields[1].Get<int32>();
            lsProficiencies[i].RequiredLevel = fields[2].Get<int32>();

            i++;
        } while (result->NextRow());

        LOG_INFO("server.loading", ">> Loaded {} proficiencies", i);
    }

    void LoadMounts()
    {
        QueryResult result = WorldDatabase.Query("SELECT `race_id`, `class_id`, `team_id`, `spell_id`, `required_level`, `required_spell_id`, `requires_quest` FROM `mod_learnspells` WHERE `type`={} ORDER BY `id` ASC", SpellType::MOUNT);

        if (!result)
        {
            LOG_INFO("server.loading", ">> Loaded 0 mounts. DB table `mod_learnspells` has no spells of type 3.");
            return;
        }

        lsMounts.clear();

        int i = 0;
        do
        {
            Field* fields = result->Fetch();

            lsMounts.push_back(Mounts());
            lsMounts[i].RaceId = fields[0].Get<int32>();
            lsMounts[i].ClassId = fields[1].Get<int32>();
            lsMounts[i].TeamId = fields[2].Get<int32>();
            lsMounts[i].SpellId = fields[3].Get<int32>();
            lsMounts[i].RequiredLevel = fields[4].Get<int32>();
            lsMounts[i].RequiredSpellId = fields[5].Get<int32>();
            lsMounts[i].RequiresQuest = fields[6].Get<int32>();

            i++;
        } while (result->NextRow());

        LOG_INFO("server.loading", ">> Loaded {} mounts", i);
    }
};

void AddLearnSpellsScripts()
{
    new LearnSpells();
    new LearnSpellsData();
}
