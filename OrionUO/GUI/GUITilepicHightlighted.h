#pragma once

#include "Core/Minimal.h"
#include "GUITilepic.h"

class CGUITilepicHightlighted : public CGUITilepic
{
    u16 SelectedColor = 0;
    bool DoubleDraw   = false;

public:
    CGUITilepicHightlighted(
        int serial, u16 graphic, u16 color, u16 selectedColor, int x, int y, bool doubleDraw);
    virtual ~CGUITilepicHightlighted();

    virtual void SetShaderMode();

    virtual void Draw(bool checktrans = false);
    virtual bool Select();

    virtual void OnMouseEnter();
    virtual void OnMouseExit();
};
