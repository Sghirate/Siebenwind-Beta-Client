#pragma once

#include "Core/Minimal.h"
#include "GUITilepic.h"
#include <string>

class CGUIMenuObject : public CGUITilepic
{
    std::string Text = "";

public:
    CGUIMenuObject(int serial, u16 graphic, u16 color, int x, int y, const std::string &text);
    virtual ~CGUIMenuObject();

    virtual bool Select();

    virtual void OnMouseEnter();
    virtual void OnMouseExit();
};
