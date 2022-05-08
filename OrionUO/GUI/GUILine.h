#pragma once

#include "Core/Minimal.h"
#include "BaseGUI.h"

class CGUILine : public CBaseGUI
{
    int TargetX = 0;
    int TargetY = 0;
    u8 ColorR   = 0;
    u8 ColorG   = 0;
    u8 ColorB   = 0;
    u8 ColorA   = 0;

public:
    CGUILine(int x, int y, int targetX, int targetY, int polygoneColor);
    virtual ~CGUILine();

    virtual void Draw(bool checktrans = false);
};
