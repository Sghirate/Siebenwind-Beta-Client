#pragma once

class CSkillGroupObject;

class CSkillGroupManager
{
    int Count = 0;

private:
    void MakeDefaultMiscellaneous();
    void MakeDefaultCombat();
    void MakeDefaultTradeSkills();
    void MakeDefaultMagic();
    void MakeDefaultWilderness();
    void MakeDefaultThieving();
    void MakeDefaultBard();

public:
    CSkillGroupObject *m_Groups{ nullptr };

    CSkillGroupManager();
    ~CSkillGroupManager();

    void MakeDefault();
    void Clear();
    void Add(CSkillGroupObject *group);
    bool Remove(CSkillGroupObject *group);
    bool Load(const std::filesystem::path& a_path);
    void Save(const std::filesystem::path& a_path);
};

extern CSkillGroupManager g_SkillGroupManager;
