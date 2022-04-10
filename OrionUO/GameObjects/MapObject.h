// MIT License
// Copyright (C) August 2016 Hotride

#pragma once

#include "../RenderWorldObject.h"

class CMapObject : public CRenderWorldObject
{
public:
    CMapObject(
        RENDER_OBJECT_TYPE type,
        int serial,
        u16 graphic,
        u16 color,
        short x,
        short y,
        char z);

    virtual ~CMapObject();
};
