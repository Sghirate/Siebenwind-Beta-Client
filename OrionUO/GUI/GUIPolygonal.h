#pragma once

#include "BaseGUI.h"

class CGUIPolygonal : public CBaseGUI
{
public:
    int Width = 0;
    int Height = 0;
    bool CallOnMouseUp = false;

    CGUIPolygonal(
        GUMP_OBJECT_TYPE type, int x, int y, int width, int height, bool callOnMouseUp = false);
    virtual ~CGUIPolygonal();
    virtual bool Select();
    virtual Core::Vec2<i32> GetSize() { return Core::Vec2<i32>(Width, Height); }
};
