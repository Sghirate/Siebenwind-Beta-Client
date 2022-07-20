#pragma once

#include "GUIDrawObject.h"

class CGUIGumppicTiled : public CGUIDrawObject
{
    int Width = 0;
    int Height = 0;

public:
    CGUIGumppicTiled(u16 graphic, int x, int y, int width, int height);
    virtual ~CGUIGumppicTiled();
    virtual Core::Vec2<i32> GetSize() { return Core::Vec2<i32>(Width, Height); }
    virtual void Draw(bool checktrans = false);
    virtual bool Select();
};
