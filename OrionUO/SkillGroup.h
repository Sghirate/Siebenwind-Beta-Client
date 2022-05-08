#pragma once

#include "Core/Minimal.h"
#include <string>

class CSkillGroupObject
{
public:
    int Count        = 0;
    bool Maximized   = false;
    std::string Name = "No Name";

private:
    u8 m_Items[60];

public:
    CSkillGroupObject* m_Next{ nullptr };
    CSkillGroupObject* m_Prev{ nullptr };

    CSkillGroupObject();
    ~CSkillGroupObject();

    u8 GetItem(intptr_t index);
    void Add(u8 index);
    void AddSorted(u8 index);
    void Remove(u8 index);
    bool Contains(u8 index);
    void Sort();
    void TransferTo(CSkillGroupObject* group);
};
