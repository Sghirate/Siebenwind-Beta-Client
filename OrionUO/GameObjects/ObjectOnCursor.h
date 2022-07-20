#pragma once

#include "Core/Minimal.h"

struct STATIC_TILES;

class CObjectOnCursor
{
public:
    bool Enabled              = false;
    bool Dropped              = false;
    u32 Serial                = 0;
    u16 Graphic               = 0;
    u16 Color                 = 0;
    u16 Count                 = 0;
    u16 TotalCount            = 0;
    u8 Layer                  = 0;
    u8 Flags                  = 0;
    u32 Container             = 0;
    u16 X                     = 0;
    u16 Y                     = 0;
    char Z                    = 0;
    bool IsGameFigure         = false;
    STATIC_TILES* TiledataPtr = nullptr;
    bool UpdatedInWorld       = false;

    CObjectOnCursor() {}
    virtual ~CObjectOnCursor() {}

    void Clear();

    u16 GetDrawGraphic(bool& doubleDraw);
};

extern CObjectOnCursor g_ObjectInHand;
