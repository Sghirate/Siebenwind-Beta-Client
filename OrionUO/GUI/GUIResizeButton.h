#pragma once

#include "GUIButton.h"

class CGUIResizeButton : public CGUIButton
{
public:
    CGUIResizeButton(
        int serial,
        u16 graphic,
        u16 graphicSelected,
        u16 graphicPressed,
        int x,
        int y);
    virtual ~CGUIResizeButton();
    virtual bool IsPressedOuthit() { return true; }
};
