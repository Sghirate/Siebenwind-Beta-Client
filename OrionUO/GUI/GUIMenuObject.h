// MIT License
// Copyright (C) August 2016 Hotride

#pragma once

#include "GUITilepic.h"

class CGUIMenuObject : public CGUITilepic
{
    string Text = "";

public:
    CGUIMenuObject(int serial, u16 graphic, u16 color, int x, int y, const std::string &text);
    virtual ~CGUIMenuObject();

    virtual bool Select();

    virtual void OnMouseEnter();
    virtual void OnMouseExit();
};
