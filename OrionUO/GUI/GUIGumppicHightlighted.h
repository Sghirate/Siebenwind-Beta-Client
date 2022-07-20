#pragma once

#include "Core/Minimal.h"
#include "GUIDrawObject.h"

class CGUIGumppicHightlighted : public CGUIDrawObject
{
    u16 SelectedColor = 0;

public:
    CGUIGumppicHightlighted(int serial, u16 graphic, u16 color, u16 electedColor, int x, int y);
    virtual ~CGUIGumppicHightlighted();
    virtual void SetShaderMode();
    virtual void OnMouseEnter();
    virtual void OnMouseExit();
};
