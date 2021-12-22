#include "Config.h"
#include "Player.h"
#include "ScriptMgr.h"

struct ClassSpells
{
    int32 RaceId;
    int32 ClassId;
    uint32 SpellId;
    uint32 RequiredLevel;
    uint32 RequiredSpellId;
    uint32 RequiresQuest;
};

struct TalentRanks
{
    int32 ClassId;
    uint32 SpellId;
    uint32 RequiredLevel;
    uint32 RequiredSpellId;
};

struct Proficiencies
{
    int32 ClassId;
    uint32 SpellId;
    uint32 RequiredLevel;
};

struct Mounts
{
    int32 RaceId;
    int32 ClassId;
    int32 TeamId;
    uint32 SpellId;
    uint32 RequiredLevel;
    uint32 RequiredSpellId;
    uint32 RequiresQuest;
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

bool enableOnLogin;
bool enableOnLevelUp;
bool enableClassSpells;
bool enableTalentRanks;
bool enableProficiencies;
bool enableFromQuests;
bool enableMaxSkill;
uint32 maxSkillMaxLevel;
bool enableRiding;
bool enableApprenticeRiding;
bool enableJourneymanRiding;
bool enableExpertRiding;
bool enableArtisanRiding;
bool enableColdWeatherFlying;

class LearnSpellsPlayer : public PlayerScript
{
    public:
        LearnSpellsPlayer() : PlayerScript("LearnSpellsPlayer") {}

        void OnLogin(Player* player) override
        {
            if (enableOnLogin)
                LearnAllSpells(player);
        }

        void OnLevelChanged(Player* player, uint8 oldLevel) override
        {
            if (enableOnLevelUp)
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

            if (enableRiding)
                LearnMountsForNewLevel(player);

            if (enableMaxSkill)
                MaxAllWeaponSkills(player);

            if (enableClassSpells && enableFromQuests && player->getClass() == CLASS_SHAMAN)
                AddShamanTotems(player);
        }

        void LearnSpellsForNewLevel(Player* player)
        {
            for (int i = 0; i < classSpells.size(); i++)
            {
                if (classSpells[i].RequiresQuest == 1 && !enableFromQuests)
                    continue;

                if (classSpells[i].RaceId == -1 || classSpells[i].RaceId == player->getRace())
                    if (classSpells[i].ClassId == player->getClass())
                        if (player->getLevel() >= classSpells[i].RequiredLevel)
                            if (classSpells[i].RequiredSpellId == -1 || player->HasSpell(classSpells[i].RequiredSpellId))
                                if (!player->HasSpell(classSpells[i].SpellId))
                                    player->learnSpell(classSpells[i].SpellId);
            }
        }

        void LearnTalentRanksForNewLevel(Player* player)
        {
            for (int i = 0; i < talentRanks.size(); i++)
            {
                if (talentRanks[i].ClassId == player->getClass())
                    if (player->getLevel() >= talentRanks[i].RequiredLevel)
                        if (player->HasSpell(talentRanks[i].RequiredSpellId))
                            if (!player->HasSpell(talentRanks[i].SpellId))
                                player->learnSpell(talentRanks[i].SpellId);
            }
        }

        void LearnProficienciesForNewLevel(Player* player)
        {
            for (int i = 0; i < proficiencies.size(); i++)
            {
                if (proficiencies[i].ClassId == player->getClass())
                    if (player->getLevel() >= proficiencies[i].RequiredLevel)
                        if (!player->HasSpell(proficiencies[i].SpellId))
                            player->learnSpell(proficiencies[i].SpellId);
            }
        }

        void MaxAllWeaponSkills(Player* player)
        {
            if (player->getLevel() <= maxSkillMaxLevel)
                player->UpdateSkillsToMaxSkillsForLevel();
        }

        void LearnMountsForNewLevel(Player* player)
        {
            for (int i = 0; i < mounts.size(); i++)
            {
                if ((mounts[i].SpellId == 33388 && !enableApprenticeRiding) || 
                    (mounts[i].SpellId == 33391 && !enableJourneymanRiding) || 
                    (mounts[i].SpellId == 34090 && !enableExpertRiding) || 
                    (mounts[i].SpellId == 34091 && !enableArtisanRiding) || 
                    (mounts[i].SpellId == 54197 && !enableColdWeatherFlying) || 
                    (mounts[i].RequiresQuest == 1 && !enableFromQuests))
                    continue;

                if (mounts[i].RaceId == -1 || mounts[i].RaceId == player->getRace())
                    if (mounts[i].ClassId == -1 || mounts[i].ClassId == player->getClass())
                        if (mounts[i].TeamId == -1 || mounts[i].TeamId == player->GetTeamId())
                            if (mounts[i].RequiredSpellId == -1 || player->HasSpell(mounts[i].RequiredSpellId))
                                if (player->getLevel() >= mounts[i].RequiredLevel)
                                    if (!player->HasSpell(mounts[i].SpellId))
                                        player->learnSpell(mounts[i].SpellId);
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

class LearnSpellsWorld : public WorldScript
{
    public:
        LearnSpellsWorld() : WorldScript("LearnSpellsWorld") { }

        void OnAfterConfigLoad(bool /*reload*/) override
        {
            enableOnLogin           = sConfigMgr->GetOption<bool>("LearnSpells.OnLogin.Enabled", 0);
            enableOnLevelUp         = sConfigMgr->GetOption<bool>("LearnSpells.OnLevelUp.Enabled", 0);
            enableClassSpells       = sConfigMgr->GetOption<bool>("LearnSpells.ClassSpells.Enabled", 0);
            enableTalentRanks       = sConfigMgr->GetOption<bool>("LearnSpells.TalentRanks.Enabled", 0);
            enableProficiencies     = sConfigMgr->GetOption<bool>("LearnSpells.Proficiencies.Enabled", 0);
            enableFromQuests        = sConfigMgr->GetOption<bool>("LearnSpells.SpellsFromQuests.Enabled", 0);
            enableMaxSkill          = sConfigMgr->GetOption<bool>("LearnSpells.MaxSkill.Enabled", 0);
            maxSkillMaxLevel        = sConfigMgr->GetOption<int32>("LearnSpells.MaxSkill.MaxLevel", 60);
            enableRiding            = sConfigMgr->GetOption<bool>("LearnSpells.Riding.Enabled", 0);
            enableApprenticeRiding  = sConfigMgr->GetOption<bool>("LearnSpells.Riding.Apprentice.Enabled", 0);
            enableJourneymanRiding  = sConfigMgr->GetOption<bool>("LearnSpells.Riding.Journeyman.Enabled", 0);
            enableExpertRiding      = sConfigMgr->GetOption<bool>("LearnSpells.Riding.Expert.Enabled", 0);
            enableArtisanRiding     = sConfigMgr->GetOption<bool>("LearnSpells.Riding.Artisan.Enabled", 0);
            enableColdWeatherFlying = sConfigMgr->GetOption<bool>("LearnSpells.Riding.ColdWeather.Enabled", 0);
        }

        void OnStartup() override
        {
            LOG_INFO("server.loading", "Loading spells...");
            {
                LoadClassSpells();
                LoadTalentRanks();
                LoadProficiencies();
                LoadMounts();
            }
        }

    private:
        void LoadClassSpells()
        {
            QueryResult result = WorldDatabase.PQuery("SELECT `race_id`, `class_id`, `spell_id`, `required_level`, `required_spell_id`, `requires_quest` FROM `mod_learnspells` WHERE `type`=%u ORDER BY `id` ASC", SpellType::CLASS);

            if (!result)
            {
                LOG_INFO("server.loading", ">> Loaded 0 class spells. DB table `mod_learnspells` has no spells of type 0.");
                return;
            }

            int i = 0;
            do
            {
                Field* fields = result->Fetch();

                classSpells.push_back(ClassSpells());
                classSpells[i].RaceId          = fields[0].GetInt32();
                classSpells[i].ClassId         = fields[1].GetInt32();
                classSpells[i].SpellId         = fields[2].GetUInt32();
                classSpells[i].RequiredLevel   = fields[3].GetUInt32();
                classSpells[i].RequiredSpellId = fields[4].GetUInt32();
                classSpells[i].RequiresQuest   = fields[5].GetUInt32();

                i++;
            } while (result->NextRow());

            LOG_INFO("server.loading", ">> Loaded %i class spells", i);
        }

        void LoadTalentRanks()
        {
            QueryResult result = WorldDatabase.PQuery("SELECT `class_id`, `spell_id`, `required_level`, `required_spell_id` FROM `mod_learnspells` WHERE `type`=%u ORDER BY `id` ASC", SpellType::TALENT);

            if (!result)
            {
                LOG_INFO("server.loading", ">> Loaded 0 talent ranks. DB table `mod_learnspells` has no spells of type 1.");
                return;
            }

            int i = 0;
            do
            {
                Field* fields = result->Fetch();

                talentRanks.push_back(TalentRanks());
                talentRanks[i].ClassId         = fields[0].GetInt32();
                talentRanks[i].SpellId         = fields[1].GetUInt32();
                talentRanks[i].RequiredLevel   = fields[2].GetUInt32();
                talentRanks[i].RequiredSpellId = fields[3].GetUInt32();

                i++;
            } while (result->NextRow());

            LOG_INFO("server.loading", ">> Loaded %i talent ranks", i);
        }

        void LoadProficiencies()
        {
            QueryResult result = WorldDatabase.PQuery("SELECT `class_id`, `spell_id`, `required_level` FROM `mod_learnspells` WHERE `type`=%u ORDER BY `id` ASC", SpellType::PROFICIENCY);

            if (!result)
            {
                LOG_INFO("server.loading", ">> Loaded 0 proficiencies. DB table `mod_learnspells` has no spells of type 2.");
                return;
            }

            int i = 0;

            do
            {
                Field* fields = result->Fetch();

                proficiencies.push_back(Proficiencies());
                proficiencies[i].ClassId       = fields[0].GetInt32();
                proficiencies[i].SpellId       = fields[1].GetUInt32();
                proficiencies[i].RequiredLevel = fields[2].GetUInt32();

                i++;
            } while (result->NextRow());

            LOG_INFO("server.loading", ">> Loaded %i proficiencies", i);
        }

        void LoadMounts()
        {
            QueryResult result = WorldDatabase.PQuery("SELECT `race_id`, `class_id`, `team_id`, `spell_id`, `required_level`, `required_spell_id`, `requires_quest` FROM `mod_learnspells` WHERE `type`=%u ORDER BY `id` ASC", SpellType::MOUNT);

            if (!result)
            {
                LOG_INFO("server.loading", ">> Loaded 0 mounts. DB table `mod_learnspells` has no spells of type 3.");
                return;
            }

            int i = 0;

            do
            {
                Field* fields = result->Fetch();

                mounts.push_back(Mounts());
                mounts[i].RaceId          = fields[0].GetInt32();
                mounts[i].ClassId         = fields[1].GetInt32();
                mounts[i].TeamId          = fields[2].GetInt32();
                mounts[i].SpellId         = fields[3].GetUInt32();
                mounts[i].RequiredLevel   = fields[4].GetUInt32();
                mounts[i].RequiredSpellId = fields[5].GetUInt32();
                mounts[i].RequiresQuest   = fields[6].GetUInt32();

                i++;
            } while (result->NextRow());

            LOG_INFO("server.loading", ">> Loaded %i mounts", i);
        }
};

void AddLearnSpellsScripts()
{
    new LearnSpellsPlayer();
    new LearnSpellsWorld();
}
