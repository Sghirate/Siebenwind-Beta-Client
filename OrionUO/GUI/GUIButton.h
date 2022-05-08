#pragma once

#include "Core/Minimal.h"
#include "GUIDrawObject.h"

class CGUIButton : public CGUIDrawObject
{
public:
    u16 GraphicSelected      = 0;
    u16 GraphicPressed       = 0;
    int ToPage               = -1;
    bool ProcessPressedState = false;

    CGUIButton(int serial, u16 graphic, u16 graphicSelected, u16 graphicPressed, int x, int y);
    virtual ~CGUIButton();

    virtual void PrepareTextures();
    virtual u16 GetDrawGraphic();

    virtual void OnMouseEnter();
    virtual void OnMouseExit();
};
