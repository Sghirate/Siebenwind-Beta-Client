#pragma once

#include "Core/Minimal.h"
#include <deque>

class CCorpse
{
public:
    u32 CorpseSerial = 0;
    u32 ObjectSerial = 0;
    u8 Direction     = 0;
    bool Running     = false;

    CCorpse() {}
    CCorpse(int corpseSerial, int objectSerial, u8 direction, bool running)
        : CorpseSerial(corpseSerial)
        , ObjectSerial(objectSerial)
        , Direction(direction)
        , Running(running)
    {
    }
    ~CCorpse() {}
};

class CCorpseManager
{
private:
    std::deque<CCorpse> m_List;

public:
    CCorpseManager() {}
    ~CCorpseManager() {}

    void Add(const CCorpse& corpse);
    void Remove(int corpseSerial, int objectSerial);
    bool InList(int corpseSerial, int objectSerial);
    class CGameObject* GetCorpseObject(int serial);
    void Clear() { m_List.clear(); }
};

extern CCorpseManager g_CorpseManager;
