#pragma once

class CGameCharacter;

class CTargetGump
{
public:
    int X = 0;
    int Y = 0;
    int Hits = 0;
    u16 Color = 0;
    u16 HealthColor = 0;
    CGameCharacter *TargetedCharacter = nullptr;
    CTargetGump();
    ~CTargetGump();

    void Draw();
};

class CNewTargetSystem
{
public:
    u32 Serial = 0;
    int X = 0;
    int TopY = 0;
    int BottomY = 0;
    int GumpX = 20;
    int GumpY = 20;
    int Hits = 0;
    u16 GumpTop = 0;
    u16 GumpBottom = 0;
    u16 ColorGump = 0;
    u16 HealthColor = 0;
    CGameCharacter *TargetedCharacter = nullptr;
    CNewTargetSystem();
    ~CNewTargetSystem();

    void Draw();
};

extern CTargetGump g_TargetGump;
extern CTargetGump g_AttackTargetGump;
extern CNewTargetSystem g_NewTargetSystem;
