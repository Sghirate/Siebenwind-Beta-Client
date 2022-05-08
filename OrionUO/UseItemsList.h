#pragma once

#include "Core/Minimal.h"
#include <deque>

class CUseItemActions
{
    u32 Timer = 0;

private:
    std::deque<u32> m_List;

public:
    CUseItemActions() {}
    ~CUseItemActions() {}

    void Add(int serial);

    void Clear() { m_List.clear(); }

    void Process();
};

extern CUseItemActions g_UseItemActions;
