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

class LearnSpellsData : public WorldScript
{
    public:
        LearnSpellsData() : WorldScript("LearnSpellsData") { }

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

class LearnSpells : public PlayerScript
{
    public:
        LearnSpells() : PlayerScript("LearnSpells") {}

        void OnLogin(Player* player) override
        {
            if (sConfigMgr->GetOption<bool>("LearnSpells.OnLogin.Enabled", 0))
                LearnAllSpells(player);
        }

        void OnLevelChanged(Player* player, uint8 oldLevel) override
        {
            if (sConfigMgr->GetOption<bool>("LearnSpells.OnLevelUp.Enabled", 0))
                LearnAllSpells(player);
        }

    private:
        void LearnAllSpells(Player* player)
        {
            if (sConfigMgr->GetOption<bool>("LearnSpells.ClassSpells.Enabled", 0))
                LearnSpellsForNewLevel(player);

            if (sConfigMgr->GetOption<bool>("LearnSpells.TalentRanks.Enabled", 0))
                LearnTalentRanksForNewLevel(player);

            if (sConfigMgr->GetOption<bool>("LearnSpells.Proficiencies.Enabled", 0))
                LearnProficienciesForNewLevel(player);

            if (sConfigMgr->GetOption<bool>("LearnSpells.Riding.Enabled", 0))
                LearnMountsForNewLevel(player);

            if (sConfigMgr->GetOption<bool>("LearnSpells.MaxSkill.Enabled", 0))
                MaxAllWeaponSkills(player);
        }

        void LearnSpellsForNewLevel(Player* player)
        {
            for (int i = 0; i < classSpells.size(); i++)
            {
                if (classSpells[i].RequiresQuest == 1 && !sConfigMgr->GetOption<bool>("SpellsFromQuests.Enabled", 0))
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
            if (player->getLevel() <= sConfigMgr->GetOption<int32>("LearnSpells.MaxSkill.MaxLevel", 60))
                player->UpdateSkillsToMaxSkillsForLevel();
        }

        void LearnMountsForNewLevel(Player* player)
        {
            for (int i = 0; i < mounts.size(); i++)
            {
                if ((mounts[i].SpellId == 33388 && !sConfigMgr->GetOption<bool>("LearnSpells.Riding.Apprentice.Enabled", 0)) || 
                    (mounts[i].SpellId == 33391 && !sConfigMgr->GetOption<bool>("LearnSpells.Riding.Journeyman.Enabled", 0)) || 
                    (mounts[i].SpellId == 34090 && !sConfigMgr->GetOption<bool>("LearnSpells.Riding.Expert.Enabled", 0)) || 
                    (mounts[i].SpellId == 34091 && !sConfigMgr->GetOption<bool>("LearnSpells.Riding.Artisan.Enabled", 0)) || 
                    (mounts[i].SpellId == 54197 && !sConfigMgr->GetOption<bool>("LearnSpells.Riding.ColdWeather.Enabled", 0)) || 
                    (mounts[i].RequiresQuest == 1 && !sConfigMgr->GetOption<bool>("LearnSpells.SpellsFromQuests.Enabled", 0)))
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
};

void AddLearnSpellsScripts()
{
    new LearnSpellsData();
    new LearnSpells();
}
