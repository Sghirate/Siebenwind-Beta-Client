#pragma once

#include "Core/Minimal.h"
#include "Core/DataStream.h"
#include "PartyObject.h"

class CParty
{
public:
    u32 Leader = 0;
    u32 Inviter = 0;
    bool CanLoot = false;
    CPartyObject Member[10];

    CParty();
    ~CParty();

    void ParsePacketData(Core::StreamReader& a_reader);
    bool Contains(int a_serial);
    void Clear();
};

extern CParty g_Party;
