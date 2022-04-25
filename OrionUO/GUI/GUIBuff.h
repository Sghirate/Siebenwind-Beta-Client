#pragma once

#include "GUIDrawObject.h"

class CGUIBuff : public CGUIDrawObject
{
public:
    u32 Timer = 0;
    std::wstring Text = {};
    u32 TooltipTimer = 0;
    bool DecAlpha = true;
    u8 Alpha = 0xFF;

    CGUIBuff(u16 graphic, int timer, const std::wstring &text);
    virtual ~CGUIBuff();
    virtual void Draw(bool checktrans = false);
};
