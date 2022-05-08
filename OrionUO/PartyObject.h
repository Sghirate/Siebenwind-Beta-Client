#pragma once

#include "Core/Minimal.h"
#include <string>

class CPartyObject
{
public:
    u32 Serial = 0;

    CPartyObject();
    ~CPartyObject() {}
    class CGameCharacter* Character{ nullptr };
    std::string GetName(int index);
};
