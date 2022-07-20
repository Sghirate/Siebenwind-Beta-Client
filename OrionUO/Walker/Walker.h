#pragma once

#include "Core/Minimal.h"
#include "Constants.h"

class CStepInfo
{
public:
    u8 Direction = 0;
    u8 OldDirection = 0;
    u8 Sequence = 0;
    bool Accepted = false;
    bool Running = false;
    bool NoRotation = false;
    u32 Timer = 0;
    u16 X = 0;
    u16 Y = 0;
    u8 Z = 0;

    CStepInfo() {}
    ~CStepInfo() {}
};

class CWalker
{
public:
    u32 LastStepRequestTime = 0;
    int UnacceptedPacketsCount = 0;
    int StepsCount = 0;
    u8 WalkSequence = 0;
    u8 CurrentWalkSequence = 0;
    bool ResendPacketSended = false;
    bool WantChangeCoordinates = false;
    bool WalkingFailed = false;
    u16 CurrentPlayerZ = 0;
    u16 NewPlayerZ = 0;

    CWalker();
    ~CWalker() {}

    CStepInfo m_Step[MAX_STEPS_COUNT];

    void Reset();
    void DenyWalk(u8 sequence, int x, int y, char z);
    void ConfirmWalk(u8 sequence);
};

extern CWalker g_Walker;
