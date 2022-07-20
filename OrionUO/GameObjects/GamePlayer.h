#pragma once

#include "GameCharacter.h"
#include "Walker/FastWalk.h"

class CPlayer : public CGameCharacter
{
public:
    short Str = 0;
    short Int = 0;
    short Dex = 0;
    u8 LockStr = 0;
    u8 LockInt = 0;
    u8 LockDex = 0;
    u16 MaxWeight = 0;
    u16 Weight = 0;
    short Armor = 0;
    u32 Gold = 0;
    bool Warmode = 0;
    u16 StatsCap = 0;
    u8 Followers = 0;
    u8 MaxFollowers = 5;
    short FireResistance = 0;
    short ColdResistance = 0;
    short PoisonResistance = 0;
    short EnergyResistance = 0;
    short MaxPhysicalResistance = 0;
    short MaxFireResistance = 0;
    short MaxColdResistance = 0;
    short MaxPoisonResistance = 0;
    short MaxEnergyResistance = 0;
    short Luck = 0;
    short MinDamage = 0;
    short MaxDamage = 0;
    u32 TithingPoints = 0;
    short DefenceChance = 0;
    short MaxDefenceChance = 0;
    short AttackChance = 0;
    short WeaponSpeed = 0;
    short WeaponDamage = 0;
    short LowerRegCost = 0;
    short SpellDamage = 0;
    short CastRecovery = 0;
    short CastSpeed = 0;
    short LowerManaCost = 0;

    short OldX = 0;
    short OldY = 0;
    char OldZ = 0;

    CPlayer(int serial);
    virtual ~CPlayer();

    CFastWalkStack m_FastWalkStack;

    void CloseBank();
    class CGameItem *FindBandage();
    bool IsPlayer() { return true; }
    virtual bool Walking() { return (LastStepTime > (u32)(g_Ticks - PLAYER_WALKING_DELAY)); }
    virtual bool NoIterateAnimIndex() { return false; }
    void UpdateAbilities();
};

extern CPlayer *g_Player;
