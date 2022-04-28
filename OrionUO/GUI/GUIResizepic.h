#pragma once

#include "GUIPolygonal.h"

class CGUIResizepic : public CGUIPolygonal
{
public:
    CGUIResizepic(int serial, u16 graphic, int x, int y, int width, int height);
    virtual ~CGUIResizepic();
    virtual Core::Vec2<i32> GetSize() { return Core::Vec2<i32>(Width, Height); }
    virtual void PrepareTextures();
    virtual void Draw(bool checktrans = false);
    virtual bool Select();
};
