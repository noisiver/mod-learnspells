#include "mod_learnspells.h"

void LearnSpells::OnStartup()
{
    LoadAllSpells();
}

void LearnSpells::LoadAllSpells()
{
    LOG_INFO("module", "Loading spells...");

    LoadClassSpells();
    LoadTalentRanks();
    LoadProficiencies();
    LoadMounts();
}

void LearnSpells::LoadClassSpells()
{
    QueryResult result = WorldDatabase.Query("SELECT `race_id`, `class_id`, `spell_id`, `required_level`, `required_spell_id`, `requires_quest` FROM `mod_learnspells` WHERE `type`={} ORDER BY `id` ASC", SpellType::CLASS);

    if (!result)
    {
        LOG_INFO("module", ">> Loaded 0 class spells. DB table `mod_learnspells` has no spells of type 0.");
        return;
    }

    ListClassSpells.clear();

    int i = 0;
    do
    {
        Field* fields = result->Fetch();

        ListClassSpells.push_back(ClassSpells());
        ListClassSpells[i].RaceId = fields[0].Get<int32>();
        ListClassSpells[i].ClassId = fields[1].Get<int32>();
        ListClassSpells[i].SpellId = fields[2].Get<int32>();
        ListClassSpells[i].RequiredLevel = fields[3].Get<int32>();
        ListClassSpells[i].RequiredSpellId = fields[4].Get<int32>();
        ListClassSpells[i].RequiresQuest = fields[5].Get<int32>();

        i++;
    } while (result->NextRow());

    LOG_INFO("module", ">> Loaded {} class spells", i);
}

void LearnSpells::LoadTalentRanks()
{
    QueryResult result = WorldDatabase.Query("SELECT `class_id`, `spell_id`, `required_level`, `required_spell_id` FROM `mod_learnspells` WHERE `type`={} ORDER BY `id` ASC", SpellType::TALENT);

    if (!result)
    {
        LOG_INFO("module", ">> Loaded 0 talent ranks. DB table `mod_learnspells` has no spells of type 1.");
        return;
    }

    ListTalentRanks.clear();

    int i = 0;
    do
    {
        Field* fields = result->Fetch();

        ListTalentRanks.push_back(TalentRanks());
        ListTalentRanks[i].ClassId = fields[0].Get<int32>();
        ListTalentRanks[i].SpellId = fields[1].Get<int32>();
        ListTalentRanks[i].RequiredLevel = fields[2].Get<int32>();
        ListTalentRanks[i].RequiredSpellId = fields[3].Get<int32>();

        i++;
    } while (result->NextRow());

    LOG_INFO("module", ">> Loaded {} talent ranks", i);
}

void LearnSpells::LoadProficiencies()
{
    QueryResult result = WorldDatabase.Query("SELECT `class_id`, `spell_id`, `required_level` FROM `mod_learnspells` WHERE `type`={} ORDER BY `id` ASC", SpellType::PROFICIENCY);

    if (!result)
    {
        LOG_INFO("module", ">> Loaded 0 proficiencies. DB table `mod_learnspells` has no spells of type 2.");
        return;
    }

    ListProficiencies.clear();

    int i = 0;
    do
    {
        Field* fields = result->Fetch();

        ListProficiencies.push_back(Proficiencies());
        ListProficiencies[i].ClassId = fields[0].Get<int32>();
        ListProficiencies[i].SpellId = fields[1].Get<int32>();
        ListProficiencies[i].RequiredLevel = fields[2].Get<int32>();

        i++;
    } while (result->NextRow());

    LOG_INFO("module", ">> Loaded {} proficiencies", i);
}

void LearnSpells::LoadMounts()
{
    QueryResult result = WorldDatabase.Query("SELECT `race_id`, `class_id`, `team_id`, `spell_id`, `required_level`, `required_spell_id`, `requires_quest` FROM `mod_learnspells` WHERE `type`={} ORDER BY `id` ASC", SpellType::MOUNT);

    if (!result)
    {
        LOG_INFO("module", ">> Loaded 0 mounts. DB table `mod_learnspells` has no spells of type 3.");
        return;
    }

    ListMounts.clear();

    int i = 0;
    do
    {
        Field* fields = result->Fetch();

        ListMounts.push_back(Mounts());
        ListMounts[i].RaceId = fields[0].Get<int32>();
        ListMounts[i].ClassId = fields[1].Get<int32>();
        ListMounts[i].TeamId = fields[2].Get<int32>();
        ListMounts[i].SpellId = fields[3].Get<int32>();
        ListMounts[i].RequiredLevel = fields[4].Get<int32>();
        ListMounts[i].RequiredSpellId = fields[5].Get<int32>();
        ListMounts[i].RequiresQuest = fields[6].Get<int32>();

        i++;
    } while (result->NextRow());

    LOG_INFO("module", ">> Loaded {} mounts", i);
}
