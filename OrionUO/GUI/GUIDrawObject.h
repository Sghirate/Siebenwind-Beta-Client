#pragma once

#include "BaseGUI.h"

class CGUIDrawObject : public CBaseGUI
{
public:
    CGUIDrawObject(
        GUMP_OBJECT_TYPE type, int serial, u16 graphic, u16 color, int x, int y);
    virtual ~CGUIDrawObject();

    virtual Core::Vec2<i32> GetSize();

    virtual void SetShaderMode();
    virtual void PrepareTextures();

    virtual void Draw(bool checktrans = false);
    virtual bool Select();
};
