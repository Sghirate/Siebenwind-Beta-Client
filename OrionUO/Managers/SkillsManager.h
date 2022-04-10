// MIT License
// Copyright (C) Obtober 2017 Hotride

#pragma once

class CSkill
{
public:
    bool Button = 0;
    string Name = "";
    float BaseValue = 0.0f;
    float Value = 0.0f;
    float Cap = 0.0f;
    u8 Status = 0;

    CSkill() {}
    CSkill(bool haveButton, const std::string &name);
    ~CSkill() {}
};

class CSkillsManager
{
public:
    u32 Count = 0;
    bool SkillsRequested = false;
    float SkillsTotal = 0.0f;

private:
    std::vector<CSkill> m_Skills;
    std::vector<u8> m_SortedTable;

    bool CompareName(const std::string &str1, const std::string &str2);

public:
    CSkillsManager() {}
    ~CSkillsManager() {}

    bool Load();
    void Add(const CSkill &skill);
    void Clear();
    CSkill *Get(u32 index);
    void Sort();
    int GetSortedIndex(u32 index);
    void UpdateSkillsSum();
};

extern CSkillsManager g_SkillsManager;
