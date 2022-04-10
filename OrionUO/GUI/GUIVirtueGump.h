// MIT License
// Copyright (C) May 2017 Hotride

#pragma once

#include "GUIDrawObject.h"

class CGUIVirtueGump : public CGUIDrawObject
{
public:
    CGUIVirtueGump(u16 graphic, int x, int y);
    virtual ~CGUIVirtueGump();
    virtual bool Select();
};
