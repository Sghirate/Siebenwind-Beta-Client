// MIT License
// Copyright (C) August 2016 Hotride

#pragma once

class CPartyObject
{
public:
    u32 Serial = 0;

    CPartyObject();
    ~CPartyObject() {}
    class CGameCharacter *Character{ nullptr };
    string GetName(int index);
};
