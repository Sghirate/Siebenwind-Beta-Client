// MIT License
// Copyright (C) August 2016 Hotride

#pragma once

#include "GameEffect.h"

class CGameObject;

class CGameEffectDrag : public CGameEffect
{
public:
    int OffsetX = 0;
    int OffsetY = 0;
    u32 LastMoveTime = 0;
    u8 MoveDelay = 20;

    CGameEffectDrag();
    virtual ~CGameEffectDrag();
    virtual void Update(CGameObject *parent);
};
