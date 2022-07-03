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

std::vector<ClassSpells> classSpells;
std::vector<TalentRanks> talentRanks;
std::vector<Proficiencies> proficiencies;
std::vector<Mounts> mounts;

enum SpellType
{
    CLASS = 0,
    TALENT,
    PROFICIENCY,
    MOUNT
};

bool enableClassSpells;
bool enableTalentRanks;
bool enableProficiencies;
bool enableFromQuests;
bool enableApprenticeRiding;
bool enableJourneymanRiding;
bool enableExpertRiding;
bool enableArtisanRiding;
bool enableColdWeatherFlying;

class LearnSpells : public PlayerScript
{
public:
    LearnSpells() : PlayerScript("LearnSpells") {}

    void OnPlayerLearnTalents(Player* player, uint32 /*talentId*/, uint32 /*talentRank*/, uint32 /*spellid*/) override
    {
        if (enableTalentRanks)
            LearnTalentRanksForNewLevel(player);
    }

    void OnLevelChanged(Player* player, uint8 /*oldLevel*/) override
    {
        LearnAllSpells(player);
    }

    void OnLogin(Player* player) override
    {
        LearnAllSpells(player);
    }
private:
    void LearnAllSpells(Player* player)
    {
        if (enableClassSpells)
            LearnSpellsForNewLevel(player);

        if (enableTalentRanks)
            LearnTalentRanksForNewLevel(player);

        if (enableProficiencies)
            LearnProficienciesForNewLevel(player);

        if (enableApprenticeRiding || enableJourneymanRiding || enableExpertRiding || enableArtisanRiding || enableColdWeatherFlying)
            LearnMountsForNewLevel(player);

        if (enableClassSpells && enableFromQuests && player->getClass() == CLASS_SHAMAN)
            AddShamanTotems(player);
    }

    void LearnSpellsForNewLevel(Player* player)
    {
        for (auto& classSpell : classSpells)
        {
            if (classSpell.RequiresQuest == 1 && !enableFromQuests)
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
        for (auto& talentRank : talentRanks)
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
        for (auto& proficiency : proficiencies)
        {
            if (proficiency.ClassId == player->getClass())
                if (player->getLevel() >= proficiency.RequiredLevel)
                    if (!player->HasSpell(proficiency.SpellId))
                        player->learnSpell(proficiency.SpellId);
        }
    }

    void LearnMountsForNewLevel(Player* player)
    {
        for (auto& mount : mounts)
        {
            if ((mount.SpellId == 33388 && !enableApprenticeRiding) ||
                (mount.SpellId == 33391 && !enableJourneymanRiding) ||
                (mount.SpellId == 34090 && !enableExpertRiding) ||
                (mount.SpellId == 34091 && !enableArtisanRiding) ||
                (mount.SpellId == 54197 && !enableColdWeatherFlying) ||
                (mount.RequiresQuest == 1 && !enableFromQuests))
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
        enableClassSpells = sConfigMgr->GetOption<bool>("LearnSpells.ClassSpells.Enabled", 1);
        enableTalentRanks = sConfigMgr->GetOption<bool>("LearnSpells.TalentRanks.Enabled", 1);
        enableProficiencies = sConfigMgr->GetOption<bool>("LearnSpells.Proficiencies.Enabled", 1);
        enableFromQuests = sConfigMgr->GetOption<bool>("LearnSpells.SpellsFromQuests.Enabled", 1);
        enableApprenticeRiding = sConfigMgr->GetOption<bool>("LearnSpells.Riding.Apprentice.Enabled", 0);
        enableJourneymanRiding = sConfigMgr->GetOption<bool>("LearnSpells.Riding.Journeyman.Enabled", 0);
        enableExpertRiding = sConfigMgr->GetOption<bool>("LearnSpells.Riding.Expert.Enabled", 0);
        enableArtisanRiding = sConfigMgr->GetOption<bool>("LearnSpells.Riding.Artisan.Enabled", 0);
        enableColdWeatherFlying = sConfigMgr->GetOption<bool>("LearnSpells.Riding.ColdWeather.Enabled", 0);

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

        classSpells.clear();

        int i = 0;
        do
        {
            Field* fields = result->Fetch();

            classSpells.push_back(ClassSpells());
            classSpells[i].RaceId = fields[0].Get<int32>();
            classSpells[i].ClassId = fields[1].Get<int32>();
            classSpells[i].SpellId = fields[2].Get<int32>();
            classSpells[i].RequiredLevel = fields[3].Get<int32>();
            classSpells[i].RequiredSpellId = fields[4].Get<int32>();
            classSpells[i].RequiresQuest = fields[5].Get<int32>();

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

        talentRanks.clear();

        int i = 0;
        do
        {
            Field* fields = result->Fetch();

            talentRanks.push_back(TalentRanks());
            talentRanks[i].ClassId = fields[0].Get<int32>();
            talentRanks[i].SpellId = fields[1].Get<int32>();
            talentRanks[i].RequiredLevel = fields[2].Get<int32>();
            talentRanks[i].RequiredSpellId = fields[3].Get<int32>();

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

        proficiencies.clear();

        int i = 0;
        do
        {
            Field* fields = result->Fetch();

            proficiencies.push_back(Proficiencies());
            proficiencies[i].ClassId = fields[0].Get<int32>();
            proficiencies[i].SpellId = fields[1].Get<int32>();
            proficiencies[i].RequiredLevel = fields[2].Get<int32>();

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

        mounts.clear();

        int i = 0;
        do
        {
            Field* fields = result->Fetch();

            mounts.push_back(Mounts());
            mounts[i].RaceId = fields[0].Get<int32>();
            mounts[i].ClassId = fields[1].Get<int32>();
            mounts[i].TeamId = fields[2].Get<int32>();
            mounts[i].SpellId = fields[3].Get<int32>();
            mounts[i].RequiredLevel = fields[4].Get<int32>();
            mounts[i].RequiredSpellId = fields[5].Get<int32>();
            mounts[i].RequiresQuest = fields[6].Get<int32>();

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
