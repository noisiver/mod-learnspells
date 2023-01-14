#ifndef MOD_LEARNSPELLS
#define MOD_LEARNSPELLS

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

enum SpellType
{
    CLASS = 0,
    TALENT,
    PROFICIENCY,
    MOUNT
};

class LearnSpells : public PlayerScript, WorldScript
{
public:
    LearnSpells();

    // PlayerScript
    void OnLevelChanged(Player* /*player*/, uint8 /*oldLevel*/) override;
    void OnLogin(Player* /*player*/) override;
    void OnPlayerLearnTalents(Player* /*player*/, uint32 /*talentId*/, uint32 /*talentRank*/, uint32 /*spellid*/) override;

    // WorldScript
    void OnAfterConfigLoad(bool /*reload*/) override;
    void OnStartup() override;

private:
    std::vector<ClassSpells> ListClassSpells;
    std::vector<TalentRanks> ListTalentRanks;
    std::vector<Proficiencies> ListProficiencies;
    std::vector<Mounts> ListMounts;

    bool EnableGamemasters;
    bool EnableClassSpells;
    bool EnableTalentRanks;
    bool EnableProficiencies;
    bool EnableFromQuests;
    bool EnableApprenticeRiding;
    bool EnableJourneymanRiding;
    bool EnableExpertRiding;
    bool EnableArtisanRiding;
    bool EnableColdWeatherFlying;

    void LearnAllSpells(Player* /*player*/);
    void LearnClassSpells(Player* /*player*/);
    void LearnTalentRanks(Player* /*player*/);
    void LearnProficiencies(Player* /*player*/);
    void LearnMounts(Player* /*player*/);
    void AddTotems(Player* /*player*/);

    void LoadAllSpells();
    void LoadClassSpells();
    void LoadTalentRanks();
    void LoadProficiencies();
    void LoadMounts();
};

#endif
