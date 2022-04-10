#pragma once

#include "Core/Minimal.h"
#include "BaseQueue.h"
#include "plugin/enumlist.h"
#include <string>

class CBaseProfession : public CBaseQueueItem
{
public:
    std::string Name             = "";
    std::string TrueName         = "";
    std::wstring Description     = {};
    u32 NameClilocID        = 0;
    u32 DescriptionClilocID = 0;
    PROFESSION_TYPE Type         = PT_NO_PROF;
    u16 Gump                = 0;
    bool TopLevel                = false;
    int DescriptionIndex         = 0;

    CBaseProfession();
    virtual ~CBaseProfession();

    bool AddDescription(int a_desc, const std::string& a_name, const char* a_val);
};

class CProfessionCategory : public CBaseProfession
{
public:
    std::string Childrens = "|";

    CProfessionCategory();
    virtual ~CProfessionCategory();

    void AddChildren(const std::string& a_child);
};

class CProfession : public CBaseProfession
{
public:
    u8 Str = 0;
    u8 Int = 0;
    u8 Dex = 0;

private:
    u8 m_SkillIndex[4];
    u8 m_SkillValue[4];

public:
    CProfession();
    virtual ~CProfession();

    void SetSkillIndex(int index, u8 val)
    {
        if (index >= 0 && index < 4)
            m_SkillIndex[index] = val;
    }
    void SetSkillValue(int index, u8 val)
    {
        if (index >= 0 && index < 4)
            m_SkillValue[index] = val;
    }

    u8 GetSkillIndex(int index) const
    {
        if (index >= 0 && index < 4)
            return m_SkillIndex[index];
        else
            return 0xFF;
    }
    u8 GetSkillValue(int index) const
    {
        if (index >= 0 && index < 4)
            return m_SkillValue[index];
        else
            return 0xFF;
    }
};
