// MIT License
// Copyright (C) August 2016 Hotride

#pragma once

#include "GUIDrawObject.h"

class CGUITilepic : public CGUIDrawObject
{
public:
    CGUITilepic(u16 graphic, u16 color, int x, int y);
    virtual ~CGUITilepic();

    virtual Core::Vec2<i32> GetSize();

    virtual void PrepareTextures();

    virtual void Draw(bool checktrans = false);
    virtual bool Select();
};
